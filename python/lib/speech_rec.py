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
            # obtain audio from the microphone
            # with sr.Microphone() as source:
            #     print("Say something!")
            #     audio = self.r.listen(source)
            
            # A partir de um arquivo de audio
            with sr.AudioFile(f"{audio_path}.wav") as source:
                audio = self.r.record(source)  # read the entire audio file


            # recognize speech using Google Speech Recognition
            try:
                # for testing purposes, we're just using the default API key
                # to use another API key, use `r.recognize_google(audio, key="GOOGLE_SPEECH_RECOGNITION_API_KEY")`
                # instead of `r.recognize_google(audio)
                voice_input = self.r.recognize_google(audio, language="pt-BR")
                print(f"Input reconhecido: {voice_input}")
         
                return self.parse_command(voice_input)

            except sr.UnknownValueError:
                # Ola em pt-br
                self.say("Eu não entendi o que voce disse")
                print("Google Speech Recognition could not understand audio")

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

        alek_interpretations = ("alek", "alex", "alexa")

        if commands[0] in alek_interpretations:
            return self.eval_command(commands)
        return


    def eval_command(self, command: str) -> None:

        print(f"Command: {command}")

        if self.stemmer.is_included_in_phrase(command, ("fim", "seman")):
            self.say("Iniciando modo fim de semana!")
            self.spotify.start_playlist()
            return 'mfs'
        elif self.stemmer.is_included_in_phrase(command, ("moda", "mod")):
            self.spotify.start_playlist()
        elif self.stemmer.is_included_in_phrase(command, ("prox", "proxim")):
            self.spotify.next_music()
        elif self.stemmer.is_included_in_phrase(command, ("volt", "voltar")):
            self.spotify.previous_music()
        elif self.stemmer.is_included_in_phrase(command, ("paus", "pausar")):
            self.spotify.pause_music()
        elif self.stemmer.is_included_in_phrase(command, ("toc", "tocar", "lig", "play")):
            self.spotify.play_music()

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