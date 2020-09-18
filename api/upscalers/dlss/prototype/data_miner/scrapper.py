import json
import os
from abc import ABC, abstractmethod

import kaggle
from termcolor import colored

from prototype.tools.consts import ROOT_DIR


class Scrapper(ABC):

    @abstractmethod
    def run(self):
        pass


class WebScrapper(Scrapper):
    def run(self):
        pass


class KaggleScrapper(Scrapper):
    def run(self):
        self.download_df()

    def download_df(self, df: str = None):
        settings_file = f"{ROOT_DIR}/settings.json"
        with open(settings_file) as json_file:
            data = json.load(json_file)
            credentials = data.get("kaggle", "")
        if not os.path.exists("../data"):
            os.makedirs("../data")
        k = kaggle.KaggleApi({"username": credentials.get("username"), "key": credentials.get("key")})
        k.authenticate()
        print("kaggle.com: authenticated")
        if df is not None:
            k.dataset_download_cli(df, unzip=True, path=f"{ROOT_DIR}/data")
        else:
            df_names = data.get("kaggle_dataset_names", "")
            for df in df_names:
                print(colored(f"downloading {df}...", "magenta"))
                k.dataset_download_cli(df, unzip=True, path=f"{ROOT_DIR}/data")
