from prototype.data_miner.scrapper import KaggleScrapper, WebScrapper
from prototype.tools.cmd import CMD


def main():
    args = get_arguments()
    action = args.action.lower()
    action_param = args.action_param
    if action == "kaggle_download":
        scrapper = KaggleScrapper()
        if action_param is None:
            scrapper.run()
        else:
            scrapper.download_df(action_param)
    if action == "web_download":
        scrapper = WebScrapper()
        if action_param is None:
            scrapper.run()
        else:
            scrapper.run_web_page(action_param)


def get_arguments():
    return CMD(
        args=[
            {"command": "--action", "type": str, "help": "Action to execute", "default": ""},
            {"command": "--action_param", "type": str, "help": "Additional param to action", "default": None}
        ]
    ).get_args()


if __name__ == "__main__":
    main()
