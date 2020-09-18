import json
import os
import time
from urllib.request import urlopen, Request
from abc import ABC, abstractmethod
from datetime import datetime
from io import StringIO
from typing import Optional
from xml.etree.ElementTree import ElementTree

import kaggle
from bs4 import BeautifulSoup
from lxml import etree
from selenium import webdriver
from selenium.webdriver.remote.webdriver import WebDriver
from termcolor import colored

from prototype.tools.consts import ROOT_DIR
from prototype.tools.utils import digits_only


class Scrapper(ABC):

    def __init__(self):
        self.settings_file: Optional[str] = f"{ROOT_DIR}/settings.json"

    @abstractmethod
    def run(self):
        pass

    def get_json(self, file_name: str):
        with open(file_name) as json_file:
            data = json.load(json_file)
        return data


class WebScrapper(Scrapper):

    def __init__(self):
        super().__init__()
        self.driver_path: Optional[str] = f"{ROOT_DIR}/bin/geckodriver"
        self.dr: Optional[WebDriver] = None
        self.url: Optional[str] = None
        self.soup_html: Optional[BeautifulSoup] = None
        self.tree_html: Optional[ElementTree] = None
        self.html = None

    def run(self):
        data = self.get_json(self.settings_file)
        urls = data.get("web_pages", "")
        for url in urls:
            print(f"downloading files from {url}...")
            self._get_driver(self.driver_path, url)
            images_path_count = 0
            images_from_view = 0
            last_images_count = 0
            to_break_counter = 0
            try:
                images_from_view = self.find_element_count()
            except Exception as e:
                print(e)
            images = []
            while images_from_view > images_path_count:
                print(
                    f"scrapping {images_path_count} of {images_from_view} || last count: {last_images_count} || break count: {to_break_counter}")
                self.scroll_to_bottom()
                time.sleep(2)
                self.page_to_html()
                try:
                    srcs = self.tree_html.findall(
                        "//a[@class='js-photo-link photo-item__link']/img")
                    img_paths = [src.attrib.get("data-big-src", None) for src in srcs]
                    images = [img[:img.rfind("?")] for img in img_paths if img.rfind("?") > 0]
                    images_path_count = len(img_paths)
                    del img_paths
                except Exception as e:
                    print(colored(e, "red"))
                if last_images_count == images_path_count:
                    to_break_counter += 1
                    if to_break_counter > 10:
                        break
                else:
                    last_images_count = images_path_count
                    to_break_counter = 0

            self.dr.quit()
            time.sleep(2)
            for indx, im in enumerate(images):
                print(colored(f"download {indx + 1} of {len(images)}", "yellow"))
                print(colored(f"downloading {im}", "cyan"))
                if ".jpg" in im:
                    extension = "jpg"
                elif ".jpeg" in im:
                    extension = "jpeg"
                else:
                    extension = "png"
                self.download_image(im, f"{ROOT_DIR}/data/web_downloads/{datetime.now()}_{indx}.{extension}")
                time.sleep(1)

    def download_image(self, url, filepath):
        req = Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        resource = urlopen(req)
        output = open(filepath, "wb")
        output.write(resource.read())
        output.close()

    def find_element_count(self):
        return [int(digits_only(e) + "0") for e in self.find_elements_inner_text_by_xpath(
            "//ul[@class='search__tabs__list search__tabs__list--rectangle']/li/a/small") if "." in e and "K" in e][0]

    def find_elements_inner_text_by_xpath(self, xpath):
        return [''.join(p.itertext()).strip() for p in self.tree_html.findall(xpath)]

    def scroll_to_bottom(self):
        self.dr.execute_script("window.scrollTo(0, document.body.scrollHeight)")

    def run_web_page(self, page: str):
        self._get_driver(self.driver_path, page)

    def page_to_html(self, html=None):
        if html is None:
            self.html = self.dr.page_source
        else:
            self.html = html
        self.soup_html = self.load_html_with_soup(self.html)
        self.tree_html = self.load_html_with_lxml(self.html)

    def load_html_with_soup(self, html: str):
        soup = BeautifulSoup(html, features="lxml")
        return soup

    def load_html_with_lxml(self, html: str):
        parser = etree.HTMLParser()
        tree = etree.parse(StringIO(html), parser)
        return tree

    def _get_driver(self, driver_path: str, url: str, driver_type: str = "firefox"):
        self.url = url
        if driver_type is "firefox":
            self.dr = webdriver.Firefox(executable_path=driver_path)
        elif driver_type is "chrome":
            self.dr = webdriver.Chrome(executable_path=driver_path)
        try:
            self.dr.get(url)
            self.page_to_html()
            return self.dr
        except Exception as e:
            raise ValueError(f"cannot start dynamic driver from type {driver_type}. {e}")


class KaggleScrapper(Scrapper):
    def run(self):
        self.download_df()

    def download_df(self, df: str = None):
        data = self.get_json(self.settings_file)
        credentials = data.get("kaggle", "")
        if not os.path.exists("../data"):
            os.makedirs("../data")
        k = kaggle.KaggleApi({"username": credentials.get("username"), "key": credentials.get("key")})
        k.authenticate()
        print("kaggle.com: authenticated")
        if df is not None:
            k.dataset_download_cli(df, unzip=True, path=f"{ROOT_DIR}/data/kaggle_downloads")
        else:
            df_names = data.get("kaggle_dataset_names", "")
            for df in df_names:
                try:
                    df_indx = df.rfind("/")
                    if df_indx > 0:
                        df_name = df[df_indx:]
                    else:
                        df_name = df
                    print(colored(f"downloading {df}...", "magenta"))
                    k.dataset_download_cli(df, unzip=True, path=f"{ROOT_DIR}/data/kaggle_downloads/{df_name}")
                except Exception as e:
                    print(colored(e, "red"))
