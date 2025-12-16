from argon2 import PasswordHasher, exceptions

STORED_HASH = (
    "$argon2id$v=19$m=16384,t=4,p=1$"
    "Z1I5ZGVRTnI0SnBWMlpicA$"
    "Vw/xil1J57cwm1LHVPljKl93cC6s4nSQLy3+TXMBnGI"
)

def main():
    ph = PasswordHasher()
    try:
        password = input("Enter password: ")
    except EOFError:
        return

    try:
        ph.verify(STORED_HASH, password)
        print("OK: password matches.")
    except exceptions.VerifyMismatchError:
        print("FAIL: password does not match.")
    except Exception as e:
        print(f"Error verifying password: {e}")

if __name__ == "__main__":
    main()