from nltk.stem.snowball import SnowballStemmer


class AlekStemmer:

    def __init__(self, language='portuguese'):
        self.stemmer = SnowballStemmer(language=language)

    def is_included_in_phrase(self, stemmed: list, options: tuple) -> bool:
        for option in options:
            if option in stemmed:
                return True
        return False

    def stem_phrase(self, phrase: str) -> list:
        words = phrase.split(" ")
        stemmed = [self.stemmer.stem(word) for word in words]
        return stemmed

    def stem_word(self, word: str) -> str:
        return self.stemmer.stem(word)
    


if __name__ == "__main__":
    stemmer = AlekStemmer()
    frase = "Alek liga o modao depois pausa e depois vai para a próxima música"
    res = stemmer.stem_phrase(frase)
    print(res)

    print(stemmer.is_included_in_phrase(res, ("modao", "moda")))
