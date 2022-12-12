import numpy as np
from scipy.io.wavfile import write
import subprocess
import matplotlib.pyplot as plt

FREQ = 8_000


def convert_to_wav(data, audio_file_name):
    """
    Escreve no formato 8-bit PCM.
    """
    src = np.array(data, dtype=np.int8)

    write(f"{audio_file_name}.wav", FREQ, src)

def play(audio_file_name):
    # Play using ffplay
    subprocess.call(["ffplay", "-nodisp", "-autoexit", f"{audio_file_name}.wav"])


def read_from_serial(data_list, audio_file_name):
    with open(f'{audio_file_name}.txt', 'r') as f:
        # read line by line
        for line in f:
            # convert to int
            if ("Amostra" not in line):
                # actual =  ((int(line) / 2047.5) - 1) * 32767 
                # data_list.append(actual)
                data_list.append(int(line))

    return data_list


def read_from_bt(audio_file_name, data_list):
        # Open a file and read it whole
        with open(f'{audio_file_name}.txt', 'r') as f:
            data = f.read()
            values = data.split(" ")
            print(values)
            for value in values:
                if value:
                    data_list.append(int(value))
        return data_list


if __name__ == '__main__':
    audio_file_name = 'teste_bt_real'
    bluetooth = True

    # Read a txt file and convert it to a list of ints
    data_list = list()
    if bluetooth:
        data_list = read_from_bt(audio_file_name, data_list)
    else:
        data_list = read_from_serial(data_list, audio_file_name)

    


    print(f"Tamanho da lista: {len(data_list)}")
    print(f"Min = {min(data_list)}\nMax = {max(data_list)}")

    # convert(data_list, audio_file_name)
    convert_to_wav(data_list, audio_file_name)

    tamanho_audio = len(data_list) / FREQ
    time_space = np.linspace(0, tamanho_audio, len(data_list))
    print(f"Tempo do audio: {tamanho_audio:.2f} segundos")

    plt.plot(time_space, data_list)
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.legend(['Audio'])
    plt.show()

    play(audio_file_name)
