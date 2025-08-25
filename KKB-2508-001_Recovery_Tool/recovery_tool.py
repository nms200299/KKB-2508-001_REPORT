from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
import os

def decryptFile(srcPath:str, dstPath:str, key:bytes):
    try:
        with open(srcPath, "rb") as file:
            cipher = file.read()
        # Enc File Read

        nonce_size = 12
        tag_size = 16
        nonce = cipher[:nonce_size]
        tag = cipher[-tag_size:]
        data = cipher[nonce_size:-tag_size]
        # Dec Setting

        decryptor = Cipher(
            algorithms.AES(key),
            modes.GCM(nonce, tag),
            backend=default_backend()
        ).decryptor()
        buf =  decryptor.update(data) + decryptor.finalize()
        # Dec AES

        dstPath = dstPath.replace(".ryk", "")
        with open(dstPath, "wb") as file:
            file.write(buf)
        # Dec File Write

        print("\t[SUC] Decrypt Success. ("+dstPath+")")
    except:
        print("\t[ERR] Decrypt Fail. ("+srcPath+")")

print("[Ransomeware Recovery Tool]")
try:
    decryptKey = bytes.fromhex(input("Input Decrypt Key : "))
    #decryptKey = b"\x13\x3a\x98\x5d\x25\x76\x5d\x4a\xf3\xc8\x4f\xcb\x1f\x82\x96\xf8\x88\xd5\xd8\xfa\x02\x86\x97\xe1\x86\x93\x9d\xba\xf2\x83\x10\x8e"
except:
    print("[ERR] Invaild Decrypt Key.")
    exit()
decryptPath = input("Input Encrypt Path : ")
savePath = '.\\RecoverySave\\'

if not os.path.isdir(savePath):
    os.mkdir(savePath)

if os.path.isdir(decryptPath):
    for root, dirs, files in os.walk(decryptPath):
        for file in files:
            if file.endswith(".ryk"):
                decryptFile(root+'\\'+file, savePath+file, decryptKey)
# Dec Dir

elif os.path.isfile(decryptPath) and decryptPath.endswith(".ryk"):
    decryptFile(decryptPath, savePath+os.path.basename(decryptPath), decryptKey)
# Dec File

else:
    print("[ERR] Wrong Path !")
# Error
