import argparse
import gzip
import hashlib
import secrets
import shutil
import tkinter as tk
from tkinter import messagebox
import socket
import traceback
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from utils import tcp_by_size as sr
import base64
import os
import subprocess


class Client:
    def __init__(self, address):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.key = ""
        try:
            self.sock.connect(address)
        except socket.error as e:
            messagebox.showerror("ERROR", f"Failed to connect to the server: {e}")
            exit(1)

        self.root = tk.Tk()
        self.root.title("NESIDO Emulator Client")
        self._create_shared_key()
        self._build_login_screen()

    def _create_shared_key(self):
        B = secrets.randbits(2048)
        try:
            print("start DH")
            G = int(sr.recv_by_size(self.sock).decode())
            print("after")
            print(G)
            P = int(sr.recv_by_size(self.sock).decode())

            Secret = pow(G,B,P)
            Number = int(sr.recv_by_size(self.sock).decode())
            sr.send_with_size(self.sock, str(Secret).encode())
            self.key = hashlib.sha256(str(pow(Number,B,P)).encode()).digest()
        except:
            messagebox.showerror("ERROR", "Failed to initialize secure communication with server")
            exit(1)
            
        
    def _build_login_screen(self):
        self.username_entry = self._create_labeled_entry("Username", 0)
        self.password_entry = self._create_labeled_entry("Password", 1, show="*")

        login_button = tk.Button(self.root, text="Login", command=self._handle_login)
        login_button.grid(row=2, column=0)

        register_button = tk.Button(self.root, text="Register", command=self._handle_register)
        register_button.grid(row=2, column=1)

    def _create_labeled_entry(self, label_text, row, **kwargs):
        label = tk.Label(self.root, text=label_text)
        label.grid(row=row, column=0)

        entry = tk.Entry(self.root, **kwargs)
        entry.grid(row=row, column=1)
        return entry

    def _handle_login(self):
        username = self.username_entry.get()
        password = self.password_entry.get()
        if username == '' or password == '':
            messagebox.showinfo('Input Error', 'Please enter both username and password')
            return

        # Send the login request to the server.
        self._send_encrypted(f"LOGIN~{username}~{password}")

        response = self._receive_decrypted().decode("utf-8")
        if response == "LOGNS~":
            self._build_game_selection_screen()
        elif response == "LOGNF~":
            messagebox.showinfo('Login result', 'Login failed')

    def _handle_register(self):
        username = self.username_entry.get()
        password = self.password_entry.get()
        if username == '' or password == '':
            messagebox.showinfo('Input Error', 'Please enter both username and password')
            return

        # Send the registration request to the server.
        self._send_encrypted(f"RGSTR~{username}~{password}")

        response = self._receive_decrypted().decode("utf-8")
        if response == "RGSTS~":
            messagebox.showinfo('Registration result', 'Registration successful')
        elif response == "RGSTF~":
            messagebox.showinfo('Registration result', 'Registration failed')

    def _build_game_selection_screen(self):
        for widget in self.root.winfo_children():
            widget.destroy()

        self.game_listbox = tk.Listbox(self.root)
        self.game_listbox.grid(row=0, column=0)
        play_button = tk.Button(self.root, text="Play", command=self._play_game)
        play_button.grid(row=1, column=0)

        # Request the list of games from the server
        self._send_encrypted("SGAME~")
        response = self._receive_decrypted().decode()
        if response[:6] == "RGAME~":
            game_list = response[6:].split('~')
            for game in game_list:
                self.game_listbox.insert(tk.END, game)
        if response[:6] == "FGAME~":
            messagebox.showinfo('Error', 'Server could not send the games')

        

    def _play_game(self):
        selected_game = self.game_listbox.get(self.game_listbox.curselection())
        print(f"Starting game: {selected_game}")
        # Starting the game
        rom_path = self._get_rom(selected_game)
        
        # Set up the environment variables for the subprocess
        env = os.environ.copy()
        env["SDL_VIDEODRIVER"] = "windows"

        # Run the emulator
        subprocess.run(["Emulator/build/nesido.exe", rom_path], env=env)
    
    def _get_rom(self, selected_game):
        
        # Request the game from the server
        self._send_encrypted(f"RQGME~{selected_game}")

        # Get the size of the incoming file
        try:
            file_size_message = self._receive_decrypted().decode("utf-8")
        except Exception as e:
            messagebox.showerror("ERROR", f"Failed to get file size from server: {str(e)}")
            return

        if file_size_message.startswith("RQGMF"):
            messagebox.showerror("ERROR", "Requested game not found on the server.")
            return

        file_size = int(file_size_message[6:])
        path = "Emulator/tmpRoms/"
        # Prepare to receive the file
        with open(path + selected_game + '.gz', 'wb') as f:
            bytes_received = 0
            chunk_counter = 0
            while bytes_received < file_size:
                try:
                    chunk = self._receive_decrypted() 
                except Exception as e:
                    messagebox.showerror("ERROR", f"Failed to receive data chunk from server: {str(e)}")
                    return
                f.write(chunk)
                bytes_received += len(chunk)
                # Send ACK to the server
                self._send_encrypted(f"ACK~{chunk_counter}")
                chunk_counter += 1
        # Decompress the file
        with gzip.open(path + selected_game + '.gz', 'rb') as f_in:
            with open(path + selected_game + '.nes', 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
            os.remove(path + selected_game + '.gz')
        return path + selected_game + '.nes'
        


    def _send_encrypted(self, message):
        cipher = AES.new(self.key, AES.MODE_CBC)
        ct_bytes = cipher.encrypt(pad(message.encode('utf-8'), AES.block_size))
        iv = cipher.iv
        ct = base64.b64encode(iv + ct_bytes).decode('utf-8')
        sr.send_with_size(ct.encode('utf-8'))

    def _receive_decrypted(self):
        try:
            encrypted_data = sr.recv_by_size(self.sock)
            if not encrypted_data:
                messagebox.showerror("ERROR", "Failed to receive data")

                return None

            iv_ct = base64.b64decode(encrypted_data)
            if len(iv_ct) < 16:
                messagebox.showerror("ERROR","Received data is too short to contain an IV")
                return None

            iv = iv_ct[:16]
            ct = iv_ct[16:]
            cipher = AES.new(self.key, AES.MODE_CBC, iv=iv)
            decrypted_message = unpad(cipher.decrypt(ct), AES.block_size)

            return decrypted_message
        except Exception as e:
            messagebox.showerror(f"ERROR","while receiving and decrypting data: {e}")
            return None

    def main(self):
        self.root.mainloop()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NESIDO Emulator Server")
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=12345)

    args = parser.parse_args()

    Client((args.host, args.port)).main()
