from lib.speech_rec import SpeechControler
from lib.spotify_controller import SpotifyController
from lib.serial_receiver import SerialControllerInterface
import argparse

spotify = SpotifyController()
speech = SpeechControler(spotify)



argparse = argparse.ArgumentParser(description="Alek - Assistente de voz")
argparse.add_argument('serial_port', type=str)
argparse.add_argument('-b', '--baudrate', type=int, default=115_200)
args = argparse.parse_args()

serial = SerialControllerInterface(args.serial_port, args.baudrate, spotify, speech)

while True:
    serial.receive()