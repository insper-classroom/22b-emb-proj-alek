import serial
import time





if __name__ == "__main__":
    serial_port = 'COM5'
    baudrate = 115_200
    ser = serial.Serial(serial_port, baudrate=baudrate)

    ser.write("AT+ORGL\r\n".encode("ascii"))

    while True:
        comando = input("Digite o comando: ")
        comando = comando + "\r\n"

        ser.write(comando.encode('ascii'))

        time.sleep(1)

        res = ser.read_all()
        
        print(res.decode('ascii'))