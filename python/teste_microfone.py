import numpy as np
from scipy.io.wavfile import write
import subprocess
import matplotlib.pyplot as plt

FREQ = 10_000


def convert(data, audio_file_name):
    src = np.array(data, dtype=np.int16)

    write(f"{audio_file_name}.wav", FREQ, src)

def convert_equalized(data, audio_file_name):
    src = np.array(data, dtype=np.float32)

    write(f"{audio_file_name}.wav", FREQ, src)

def play(audio_file_name):
    # Play using ffplay
    subprocess.call(["ffplay", "-nodisp", "-autoexit", f"{audio_file_name}.wav"])



if __name__ == '__main__':
    audio_file_name = 'teste_5'

    # Read a txt file and convert it to a list of ints
    data_list = list()
    with open(f'{audio_file_name}.txt', 'r') as f:
        # read line by line
        for line in f:
            # convert to int
            if ("Amostra" not in line):
                actual =  (int(line) / 2047.5) - 1
                data_list.append(actual)
                # data_list.append(int(line))

    print(f"Tamanho da lista: {len(data_list)}")

    # convert(data_list, audio_file_name)
    convert_equalized(data_list, audio_file_name)

    tamanho_audio = len(data_list) / FREQ
    time_space = np.linspace(0, tamanho_audio, len(data_list))
    print(f"Tempo do audio: {tamanho_audio:.2f} segundos")

    plt.plot(time_space, data_list)
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.legend(['Audio'])
    plt.show()

    play(audio_file_name)
