#!/usr/bin/env python3
import speech_recognition as sr
import unicodedata
import subprocess
from gtts import gTTS
from lib.stemmer import AlekStemmer

class SpeechControler:

    def __init__(self, spotify_controller) -> None:
        self.spotify = spotify_controller 
        self.r = sr.Recognizer()
        self.stemmer = AlekStemmer()


    def recognize_speech(self, audio_path) -> None:
            
            # A partir de um arquivo de audio
            with sr.AudioFile(f"{audio_path}.wav") as source:
                audio = self.r.record(source)  # read the entire audio file


            # recognize speech using Google Speech Recognition
            try:
                voice_input = self.r.recognize_google(audio, language="pt-BR")
                print(f"Input reconhecido: {voice_input}")
         
                return self.parse_command(voice_input)

            except sr.UnknownValueError:
                self.say("Eu não entendi o que voce disse")

            except sr.RequestError as e:
                print("Could not request results from Google Speech Recognition service; {0}".format(e))

    
    def say(self, text: str) -> None:
        tts = gTTS(text, lang="pt", tld='com.br')
        temp_address = "temp.mp3"
        tts.save(temp_address)
        self.play_audio(temp_address)
        

    def parse_command(self, command: str) -> None:
        
        command = self.remove_graphic_accent(command.lower())

        commands = self.stemmer.stem_phrase(command)

        print(commands)

        alek_interpretations = ("alek", "alex", "alexa", "alec")

        if commands[0] in alek_interpretations:
            return self.eval_command(commands)
        return


    def eval_command(self, command: str) -> None:
        """
        Lista de commandos: 
        - "Alek, Modo fim de semana": Liga Led, toca a playlist e ativa o rele na placa.
        - "Alek, Proxima musica": Toca a proxima musica da playlist.
        - "Alek, volta": Toca a musica anterior da playlist.
        - "Alek, Pausa": Pausa a musica.
        - "Alek, toca" Ou "Alek, play": Toca a atual na fila do spotify.
        - "Alek, Desliga": Desliga o rele e led na placa e a musica.
        """

        print(f"Command: {command}")

        if self.stemmer.is_included_in_phrase(command, ("fim", "seman", "mod", "moda")):
            self.say("Iniciando modo fim de semana!")
            self.spotify.start_playlist()
            return 'mfs'
        elif self.stemmer.is_included_in_phrase(command, ("prox", "proxim", "pros", "seguint")):
            self.spotify.next_music()
        elif self.stemmer.is_included_in_phrase(command, ("volt", "voltar", "vol")):
            self.spotify.previous_music()
        elif self.stemmer.is_included_in_phrase(command, ("paus", "pausar", "par", "pal", "pau", "paull")):
            self.spotify.pause_music()
        elif self.stemmer.is_included_in_phrase(command, ("toc", "tocar", "lig", "play")):
            self.spotify.play_music()
        elif self.stemmer.is_included_in_phrase(command, ("encerr", "deslig")):
            self.say("Ok, desligando") 
            self.spotify.pause_music()
            return 'f'

    def play_audio(self, audio_path: str) -> None:
        subprocess.call(["ffplay", audio_path, "-nodisp", "-autoexit"])
        

    def remove_graphic_accent(self, command: str) -> str:
        command = unicodedata.normalize("NFD", command).encode("ascii", "ignore")
        return command.decode("utf-8")

if __name__ == "__main__":
    speech = SpeechControler("teste_4.wav", None)
    # speech.run()
    # Testando stemmer
    frase = "Alek liga o modao depois pausa e depois vai para a próxima música voltar para a música anterior"
    speech.parse_command(frase)