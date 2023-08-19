# import time
# import sys
#
# while True:
#     time.sleep(1)
#     print("Running...")
#     inp = sys.stdin.read()
#     if inp


import sys
import select


while True:
    try:
        if select.select([sys.stdin,],[],[],2.0)[0]:
            line = sys.stdin.readline()
            print(f"Got: {line}", flush=True)
        else:
            print("No data for 2 secs", flush=True)

    except StopIteration:
        print('EOF!')
        break
