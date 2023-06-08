import argparse
import gzip
import hashlib
import shutil
import sqlite3
import os
import socket
import sys
from threading import Thread, Lock
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from utils import tcp_by_size as sr
import base64
import secrets
import time

db_lock = Lock()

class UserDatabase:
    def __init__(self, db_path='users.db'):
        self.db_path = db_path
        with db_lock:
            db = sqlite3.connect(db_path)
            cursor = db.cursor()
            cursor.execute(
                'CREATE TABLE IF NOT EXISTS users(username TEXT PRIMARY KEY, password TEXT, salt TEXT)')
            db.commit()
            db.close()

    def register_user(self, username, password):
        with db_lock:
            db = sqlite3.connect(self.db_path)
            cursor = db.cursor()
            try:
                salt = secrets.token_bytes(16)  # create a new salt for this user
                hashed_password = hashlib.sha256((password.encode() + salt)).hexdigest()
                cursor.execute(
                    'INSERT INTO users VALUES (?, ?, ?)', (username, hashed_password, salt.hex()))
                db.commit()
                return True
            except sqlite3.IntegrityError:
                # This exception will be raised if the username is already taken
                return False
            finally:
                db.close()

    def login_user(self, username, password):
        with db_lock:
            db = sqlite3.connect(self.db_path)
            cursor = db.cursor()
            cursor.execute(
                'SELECT * FROM users WHERE username=?', (username,))
            user = cursor.fetchone()
            db.close()

            if user is None:
                return False  # user not found

            salt = bytes.fromhex(user[2])  # get the salt
            hashed_password = hashlib.sha256((password.encode() + salt)).hexdigest()

            return user[1] == hashed_password  # check the password


class Server:
    def __init__(self, roms_path, host, port):
        self.host = host
        self.port = port
        self.server = None
        self.clients = []
        self.db = UserDatabase()
        self.roms_path = roms_path
        self.running = True

    def start(self):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((self.host, self.port))
        self.server.listen()

        print(f"Server started, listening on {self.host}:{self.port}")

        while self.running:
            try:
                client_sock, addr = self.server.accept()
                print(f"New client connected from {addr}")

                client_thread = Thread(target=self.handle_client, args=(client_sock,))
                client_thread.start()

                self.clients.append((client_sock, addr))
            except socket.timeout:
                continue
            except Exception as e:
                print(f"An error occurred: {e}")

    def stop(self):
        self.running = False
        for client_sock, _ in self.clients:
            client_sock.close()
        self.server.close()

    def handle_client(self, client_sock):
        try:
            G = 5
            P = 0xFFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF6955817183995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200CBBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFCE0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B2699C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C93402849236C3FAB4D27C7026C1D4DCB2602646DEC9751E763DBA37BDF8FF9406AD9E530EE5DB382F413001AEB06A53ED9027D831179727B0865A8918DA3EDBEBCF9B14ED44CE6CBACED4BB1BDB7F1447E6CC254B332051512BD7AF426FB8F401378CD2BF5983CA01C64B92ECF032EA15D1721D03F482D7CE6E74FEF6D55E702F46980C82B5A84031900B1C9E59E7C97FBEC7E8F323A97A7E36CC88BE0F1D45B7FF585AC54BD407B22B4154AACC8F6D7EBF48E1D814CC5ED20F8037E0A79715EEF29BE32806A1D58BB7C5DA76F550AA3D8A1FBFF0EB19CCB1A313D55CDA56C9EC2EF29632387FE8D76E3C0468043E8F663F4860EE12BF2D5B0B7474D6E694F91E6DCC4024FFFFFFFFFFFFFFFF
            A = secrets.randbits(2048)
            sr.send_with_size(client_sock, str(G).encode())
            time.sleep(1)
            sr.send_with_size(client_sock, str(P).encode())
            x = pow(G,A,P)
            sr.send_with_size(client_sock,str(x).encode())
            y = sr.recv_by_size(client_sock)
            if len(y) == 0:
                raise Exception
            y = int(y.decode())
            key = hashlib.sha256(str(pow(y,A,P)).encode()).digest()
        except Exception as e:
            print(f"An error occurred while handling a client: {e}")
            client_sock.close()
            return
        while True:
            try:
                decrypted_message = self._receive_decrypted(client_sock,key)
                command = decrypted_message.split("~")[0]
                if command == "LOGIN":
                    username = decrypted_message.split("~")[1]
                    password = decrypted_message.split("~")[2]
                    if self.db.login_user(username, password):
                        self._send_encrypted(client_sock, key, "LOGNS~")
                    else:
                        self._send_encrypted(client_sock, key, "LOGNF~")

                elif command == "RGSTR":
                    username = decrypted_message.split("~")[1]
                    password = decrypted_message.split("~")[2]
                    if self.db.register_user(username, password):
                        self._send_encrypted(client_sock, key, "RGSTS~")
                    else:
                        self._send_encrypted(client_sock, key, "RGSTF~")
                
                # Game retrieval commands here
                elif command == "SGAME":
                    games = os.listdir(self.roms_path)
                    if len(games) != 0:
                        ret_message = "RGAME"
                        for game in games:
                            ret_message += '~' + game.split('.')[0]
                    else:
                        ret_message = "FGAME~"
                    self._send_encrypted(client_sock,key,ret_message)

                elif command == "RQGME":
                    game_name = decrypted_message.split("~")[1] + '.nes'
                    game_file_path = os.path.join(self.roms_path, game_name)
                    print(game_file_path)
                    if os.path.isfile(game_file_path):
                        # Compress the file first
                        try:
                            with open(game_file_path, 'rb') as f_in:
                                with gzip.open(game_file_path + '.gz', 'wb') as f_out:
                                    shutil.copyfileobj(f_in, f_out)
                        except Exception as e:
                            print(f"Error: failed to open or compress game file: {str(e)}")
                            self._send_encrypted(client_sock, key, "RQGMF~")
                            return
                        game_file_path += '.gz'
                        file_size = os.stat(game_file_path).st_size
                        self._send_encrypted(client_sock, key, f"FLSZE~{file_size}")
                        # Send the file in chunks
                        with open(game_file_path, 'rb') as f:
                            chunk_counter = 0
                            while True:
                                bytes_read = f.read(4096)
                                if not bytes_read:
                                    # File sending is done
                                    break
                                self._send_encrypted(client_sock, key, bytes_read)
                                # Wait for ACK from client
                                try:
                                    ack, chunk_num = self._receive_decrypted(client_sock, key).split("~")
                                    if ack != "ACK" or int(chunk_num) != chunk_counter:
                                        print(f"Error: did not receive correct ACK from client for chunk {chunk_counter}")
                                        break
                                except Exception as e:
                                    print(f"Error: failed to receive ACK from client: {str(e)}")
                                    break
                                chunk_counter += 1
                        os.remove(game_file_path)  # remove the compressed game file
                    else:
                        self._send_encrypted(client_sock, key, "RQGMF~")  # game not found
                elif command == "CLOSE":
                    client_sock.close()
                    return

            except Exception as e:
                print(f"Connection to the client lost")
                client_sock.close()
                break
        
    def _send_encrypted(self, client_sock, key, message):
        cipher = AES.new(key, AES.MODE_CBC)
        if isinstance(message, str):
            message = message.encode('utf-8')
        ct_bytes = cipher.encrypt(pad(message, AES.block_size))
        iv = cipher.iv
        ct = base64.b64encode(iv + ct_bytes).decode('utf-8')
        sr.send_with_size(client_sock, ct.encode('utf-8'))
        print ("\nSent(%s)>>>" % (len(message),), end='')
        print ("%s"%(message[:min(len(message),100)],))

    def _receive_decrypted(self, client_sock, key):
        while True:
            try:
                encrypted_data = sr.recv_by_size(client_sock)
                break
            except socket.timeout:
                continue
            except socket.error:
                raise Exception

        if encrypted_data == b'':
            raise Exception
        
        iv_ct = base64.b64decode(encrypted_data)
        iv = iv_ct[:16]
        ct = iv_ct[16:]
        cipher = AES.new(key, AES.MODE_CBC, iv=iv)
        decrypted_message = unpad(cipher.decrypt(ct), AES.block_size).decode('utf-8')
        print ("\nRecv(%s)>>>" % (decrypted_message[:6],), end='')
        print ("%s"%(decrypted_message[:min(len(decrypted_message),100)],))
        return decrypted_message
        

def stop_server(server):
    while True:
        command = input()
        if command == "stop":
            server.stop()
            break

def main():
    parser = argparse.ArgumentParser(description="NESIDO Emulator Server")
    parser.add_argument('--roms_path', default='roms')
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=12345)

    args = parser.parse_args()

    server = Server(args.roms_path, args.host, args.port)
    try:
        stop_thread = Thread(target=stop_server, args=(server,))
        stop_thread.start()

        server.start()
    except KeyboardInterrupt:
        print("\nStopping server...")
        server.stop()
    except Exception as e:
        print(f"An error occurred: {e}")
        server.stop()

if __name__ == "__main__":
    main()

