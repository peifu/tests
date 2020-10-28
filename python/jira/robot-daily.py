#!/usr/bin/env python3
#coding: utf-8

import time
import json
import jira_client as jc
import mail_client as mc

JIRA_FILTER = "cfg/jira_filter.json"
MAIL_RECEIVER = [
    "jiangpeifu@126.com", 
    "peifu.jiang@amlogic.com"
]
MAIL_SUBJECT = "SECURITY TEAM JIRA LIST Weekly Report"

HTML_HEAD2 = """
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <title>Jira Summary</title>
        <style type="text/css">
            *{
                margin: 0;
                padding: 0;
            }
            body{
                font: italic 18px Georgia, serif;
                letter-spacing: normal;
            }
            table{
                font: bold 16px/1.4em Georgia, serif, sans-serif;
            }
            table thead th{
                padding: 15px;
                border: 1px solid #93CE37;
                border-bottom: 3px solid #9ED929;
                text-shadow: 1px 1px 1px #568F23;
                color: #fff;
                background-color: #9DD929;
                border-radius: 5px 5px 0px 0px;
            }
            table thead th:empty{
                background-color: transparent;
                border: none;
            }
            table tbody th{
                padding: 0px 10px;
                border: 1px solid #93CE37;
                border-right: 3px solid #9ED929;
                text-shadow: 1px 1px 1px #568F23;
                color: #666;
                background-color: #9DD929;
                border-radius: 5px 0px 0px 5px;
            }
            table tbody td{
                padding: 10px;
                border: 2px solid #E7EFE0;
                text-align: center;
                text-shadow: 1px 1px 1px #fff;
                color: #666;
                background-color: #DEF3CA;
                border-radius: 2px;
            }
        </style>
    </head>
    <body>
        <p><b>Guys,</b><br><br>
        Please take a look at your own Jira. Please handle this ASAP!
        <br>
        </p>
        """
HTML_TAIL2 = """
        <address>
            <br>
            Thanks,
            <br><a href="mailto:peifu.jiang@amlogic.com">Peifu Jiang</a><br>
        </address>
    </body>
    </html>
    """

HTML_HEAD = """\
<html>
<head></head>
<body>
  <p><b>Dear All</b>,<br><br>
    Here is Jira list summary of our team.<br>
    Please check your own list and handle it ASAP!<br><br>
  </p>
"""

HTML_TAIL = """\
  <p><br><br>
    Thanks,<br>
    -Peifu<br>
    <br>
  </p>
</body>
</html>
"""

def get_filters(cfg):
    f = open(cfg, encoding="utf-8")
    filter_json = json.load(f)
    f.close()
    return filter_json["filter"]

receiver = MAIL_RECEIVER
subject = MAIL_SUBJECT + " - " + time.strftime("%Y%m%d")
html = HTML_HEAD2
filters = get_filters(JIRA_FILTER)
for filter in filters:
    jira_title = "<br><h5>{}<br></h5>".format(filter["name"])
    jira_table = jc.get_jira_html(filter)
    html += jira_title
    html += jira_table
html += HTML_TAIL2
mc.send_mail(receiver, subject, "", html)
