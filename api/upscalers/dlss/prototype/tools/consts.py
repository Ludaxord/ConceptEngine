import os
from pathlib import Path

home = str(Path.home())

ROOT_DIR = str(Path(os.path.dirname(os.path.abspath(__file__))).parent)

driver_path = f"{ROOT_DIR}/bin/geckodriver"

selenium_params = [
    "--no-sandbox",
    "--headless",
    "--disable-infobars",
    "--disable-extensions",
    "--disable-gpu",
    "--disable-dev-shm-usage",
    "--whitelisted-ips",
    "--verbose",
    "disable-infobars",
]