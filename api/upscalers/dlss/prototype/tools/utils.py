import re
import pandas as pd
from tensorflow.python.keras.losses import binary_crossentropy
from keras import backend as K


def clean_html(raw_html):
    cleanr = re.compile('<.*?>')
    cleantext = re.sub(cleanr, ' ', raw_html)
    return cleantext


def clean_html_with_tags(raw_html):
    cleanr = re.compile('<.*?>|&([a-z0-9]+|#[0-9]{1,6}|#x[0-9a-f]{1,6});')
    cleantext = re.sub(cleanr, ' ', raw_html)
    return cleantext


def clean_spaces_chars(raw_string):
    cleanr = re.compile('(^[ \t]+|[ \t]+(?=:))')
    cleantext = re.sub(cleanr, ' ', raw_string)
    return cleantext


def custom_loss(y_true, y_pred):
    return binary_crossentropy(K.reshape(y_true[:, 0], (-1, 1)), y_pred) * y_true[:, 1]


def concat_one_char_strings(raw_list):
    c = ""
    concat = []
    for r in raw_list:
        if len(r) == 1 and r.isupper():
            c += r
        else:
            concat.append(c)
            concat.append(r)
            c = ""
    return concat


def split_capitalize(raw_string):
    split = re.findall('[A-Z][^A-Z]*', raw_string)
    return split


def convert_string_to_list(string_list):
    list_of_strings = []
    for t in string_list:
        if isinstance(t, str):
            t = t.strip('][').split(', ')
        list_of_strings.append(t)
    return list_of_strings


def print_full(x):
    pd.set_option('display.max_rows', len(x))
    print(x)
    pd.reset_option('display.max_rows')


def with_full_df(df):
    with pd.option_context('display.max_rows', None, 'display.max_columns', None):  # more options can be specified also
        print(df)


class SentenceGetter(object):

    def __init__(self, data, agg_func, group_column):
        self.n_sent = 1
        self.data = data
        self.empty = False

        # A lambda function can take any number of arguments, but can only have one expression.
        # agg_func = lambda s: [(w, p, t) for w, p, t in
        # zip(s["Word"].values.tolist(), s["POS"].values.tolist(), s["Tag"].values.tolist())]
        # self.grouped = self.data.groupby("Sentence #").apply(agg_func)

        self.grouped = self.data.groupby(group_column).apply(agg_func)
        self.sentences = [s for s in self.grouped]

    def get_next(self):
        try:
            s = self.grouped["Sentence: {}".format(self.n_sent)]
            self.n_sent += 1
            return s
        except:
            return None
