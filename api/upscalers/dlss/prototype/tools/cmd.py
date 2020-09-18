import argparse


class CMD:
    parser = None
    args = []

    def __init__(self, args=None):
        if args is None:
            args = []
        self.parser = self.__init_parser()
        self.__add_args(args)
        self.args = self.parser.parse_args()

    def get_parser(self):
        return self.parser

    def get_args(self):
        return self.args

    def __init_parser(self):
        parser = argparse.ArgumentParser()
        return parser

    def __add_args(self, args):
        for arg in args:
            if isinstance(arg, dict):
                arg_command = arg.get("command")
                arg_nargs = arg.get("nargs")
                arg_action = arg.get("action")
                arg_type = arg.get("type")
                arg_help = arg.get("help")
                arg_metavar = arg.get("metavar")
                default = arg.get("default")
                self.parser.add_argument(arg_command, type=arg_type, nargs=arg_nargs, action=arg_action, help=arg_help,
                                         metavar=arg_metavar, default=default)


class StoreDictKeyPair(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        my_dict = {}
        for kv in values.split(","):
            k, v = kv.split("=")
            my_dict[k] = v
        setattr(namespace, self.dest, my_dict)
