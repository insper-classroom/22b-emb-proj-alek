import serial
import time
from scipy.io.wavfile import write
import numpy as np

# Activate DEBUG prints
DEBUG = True


class SerialControllerInterface:
    # Protocolo
    # Primeiro byte -> commando ('S' para sons, 'b' para botões)
    # segundo ate setimo byte -> Se for som, indica tamanhdo do audio, se for botão, indica o botão
    # Ultimo byte -> EOP - End of Package -> valor reservado 'X'

    def __init__(self, port, baudrate, spotify_controller, speech_recognizer):
        self.ser = serial.Serial(port, baudrate=baudrate)
        self.spotify_controller = spotify_controller
        self.speech_recognizer = speech_recognizer

        self.handshake()


    def handshake(self):
        while True:
            self.ser.write(b'H')
            time.sleep(0.5)
            if self.ser.read() == b'O':
                DEBUG and print("Terminou o handshake")
                break


    def wait_awake(self):
        while True:
            button = self.ser.read()

            if self.ser.read() == b'L':
                DEBUG and print("Saiu do modo sleep")
                eof = self.ser.read()
                if eof != b'X':
                    print("Erro de protocolo")
                break
            time.sleep(0.5)

    def convert_to_wav(self, data, audio_file_name, freq):
        """
        Escreve no formato 8-bit PCM.
        """
        src = np.array(data, dtype=np.int8)

        write(f"{audio_file_name}.wav", freq, src)


    def receive(self):
        command = self.ser.read()
        DEBUG and print(f"Commando recebido: {command}")

        if command == b'S':
            data_list = list()

            # Tamanho de 4 bytes, mais um ultimo para indicar que a leitura foi feita corretamente
            tamanho = self.ser.read(4)
            tamanho = int.from_bytes(tamanho, 'little')
            DEBUG and print(f"{tamanho = }")
            end_tamanho = self.ser.read()
            if end_tamanho != b'T':
                print("Erro na leitura do tamanho")
            
            print(tamanho)
            for _ in range(tamanho):
                data = self.ser.read()
                data_list.append(int.from_bytes(data, 'little'))
                # DEBUG and print(data)
        
            data_final = data_list
        if command == b'b':
            botao = self.ser.read()
            data_final = botao
            print(botao)

        eof = self.ser.read()
        if eof != b'X':
            print("Erro de protocolo")

        self.handle_command(command, data_final)

    def handle_command(self, command, data):
        if command == b'S':
            audio_path = "audio"
            self.convert_to_wav(data, audio_path, 8_000)

            res = self.speech_recognizer.recognize_speech(audio_path)

            if res == 'mfs':
                self.ser.write(b'F')
            if res == 'f':
                self.ser.write(b'f')


        elif command == b'b':
            try:
                match data:
                    case b'L':
                        self.wait_awake()
                    case b'P':
                        self.spotify_controller.pause_play_toggle()
                    case b'R':
                        self.spotify_controller.next_music()
                    case b'T':
                        self.spotify_controller.previous_music()
            except Exception as e:
                # Spotipy da throw em muito erro, que muitas vezes não interfere com o funcionamento do programa
                print(e)






if __name__ == '__main__':
    serial_port = 'COM7'
    baudrate = 115200
    serial_obj = SerialControllerInterface(serial_port, baudrate, None, None)

    while True:
        serial_obj.receive()