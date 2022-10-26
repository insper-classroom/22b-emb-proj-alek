import spotipy
from spotipy.oauth2 import SpotifyOAuth
from dotenv import load_dotenv
import os
import time


class SpotifyController:
    def __init__(self) -> None:
        # Carrega as credenciais do arquivo .env
        load_dotenv()
        # Permissoes necessarias
        scopes = ["user-modify-playback-state", "user-read-playback-state"]
        # Inicializa o spotipy e seleciona o dispositivo
        self.sp = spotipy.Spotify(auth_manager=SpotifyOAuth(scope=scopes))
        self.select_device()


    def next_music(self) -> None: 
        self.sp.next_track(self.device_id)

    def is_playing(self) -> bool:
        return self.sp.current_playback()['is_playing']

    def pause_play_toggle(self) -> None:
        if self.is_playing():
            self.pause_music()
        else:
            self.play_music()

    def previous_music(self) -> None:
        self.sp.previous_track(self.device_id)

    def pause_music(self) -> None:
        if self.is_playing():
            self.sp.pause_playback(self.device_id) 

    def play_music(self) -> None:
        if not self.is_playing():
            self.sp.start_playback(self.device_id)

    def start_playlist(self, playlist_id=None) -> None:
            # Playlist padrao -> Playlist de modao.
            uri = 'spotify:playlist:0sZch3HZnajbOsr0HzsFQe'
            if playlist_id:
                uri = f"spotify:playlist:{playlist_id}"
            self.sp.start_playback(self.device_id, context_uri=uri)
            self.sp.shuffle(True, self.device_id)

    def select_device(self) -> None:
        devices = self.sp.devices()
        if not devices:
            raise Exception("Nenhum dispositivo encontrado")
        for i, device in enumerate(devices['devices']):
            print(f"{i}: {device['name']}")
        
        device_id = int(input("Select device number: "))

        self.device_id = devices['devices'][device_id]['id']


if __name__ == "__main__":
    spotify = SpotifyController()


    while True:
        command = input("Enter command: ")

        if command == "modao":
            spotify.start_playlist()
        if command == "next":
            spotify.next_music()
        elif command == "previous":
            spotify.previous_music()
        elif command == "pause":
            spotify.pause_music()
        elif command == "play":
            spotify.play_music()
        elif command == "toggle":
            spotify.pause_play_toggle()


        print(spotify.sp.current_playback())