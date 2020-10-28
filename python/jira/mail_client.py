#!/usr/bin/env python3
#coding: utf-8

import zmail
import json
import time

DEBUG_ENABLE = 0

MAIL_CONFIG = "cfg/mail_server.json"
MAIL_SAVE = "email/email_{}.html"

TEST_RECEIVER = "peifu.jiang@amlogic.com"
TEST_SUBJECT = "Python Email Test"
TEST_TEXT_CONTENT = "Hello, this is a python mail test!"

def debug(args):
    if DEBUG_ENABLE == 1:
        print(args)

def init_server(cfg):
    f = open(cfg, encoding="utf-8")
    config = json.load(f)
    f.close()
    debug(config)
    return config["server"]

def init_mail_stmp(server):
    mail = zmail.server(server["user"], server["password"], smtp_host=server["smtp_server"], smtp_port=server["smtp_port"], smtp_ssl=False)
    return mail

def init_mail_pop(server):
    mail = zmail.server(server["user"], server["password"], pop_host=server["pop_server"], pop_port=server["pop_port"], pop_tls=False)
    return mail

def send_mail(receiver, subject="", text="", html="", attachment=""):
    server = init_server(MAIL_CONFIG)
    mail = init_mail_stmp(server)
    mail_to = receiver
    mail_content = {
        "subject": subject,
        "content_text": text,
        "content_html": html,
        "attachments": attachment
    }
    mail.send_mail(mail_to, mail_content)

def receive_mail(count=1):
    server = init_server(MAIL_CONFIG)
    mail = init_mail_pop(server)
    #return mail.get_mail(count)
    return mail.get_latest()

def save_mail(buff):
    dt = time.strftime("%Y-%m-%d %H:%M")
    filepath = MAIL_SAVE.format(dt)
    fp = open(filepath, "w+")
    fp.write(buff)
    fp.close()

def show_mail(mail):
    zmail.show(mail)

def dump_mail(mail):
    for k,v in mail.items():
       print(k,v)

def test_send_mail():
    send_mail(TEST_RECEIVER, TEST_SUBJECT, TEST_TEXT_CONTENT)

def test_receive_mail():
    mail = receive_mail(1)
    show_mail(mail)

if __name__ == "__main__":
    # test send mail
    test_send_mail()

