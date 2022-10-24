import serial
import time


class SerialControllerInterface:

    # Protocolo
    # Primeiro byte -> commando ('S' para sons, 'b' para botões)
    # Segundo byts -> Se for som, indica tamanhdo do audio, se for botão, indica o botão
    # Ultimo byte -> EOP - End of Package -> valor reservado 'X'

    def __init__(self, port, baudrate, spotify_controller, speech_recognizer):
        self.ser = serial.Serial(port, baudrate=baudrate)

        self.handshake()


    def handshake(self):
        while self.ser.read() != b'O':
            self.ser.write(b'H')
            time.sleep(0.5)


    def receive(self):
        command = self.ser.read()
        if command == b'S':
            data_list = list()
            # Por equanto, 1 byte para o tamanho do audio. porem talvez usar 4 bytes
            tamanho = self.ser.read()
            print(tamanho)
            for _ in tamanho:
                data = self.ser.read()
                data_list.append(int(data.decode('decimal')))
                print(data)
        if command == b'b':
            botao = self.ser.read()
            print(botao)

        eof = self.ser.read()
        if eof != b'X':
            print("Erro de protocolo")

    def handle_command(self, command, data):
        if command == b'S':
            # TODO - ler auidio e agir sobre cada um dos comandos
            self.speech_recognizer.recognize(data)

        elif command == b'b':
            try:
                match data:
                    case b'P':
                        self.spotify_controller.pause_play_toggle()
                    case b'R':
                        self.spotify_controller.next()
                    case b'T':
                        self.spotify_controller.previous()
            except Exception as e:
                # Spotipy da throw em muito erro, que muitas vezes não interfere com o funcionamento do programa
                print(e)






if __name__ == '__main__':
    serial_port = 'COM3'
    baudrate = 9600
    serial = SerialControllerInterface(serial_port, baudrate)

    while True:
        serial.receive()