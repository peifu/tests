#!/usr/bin/env python3
#coding: utf-8

import time
import json
import jira_client as jc
import mail_client as mc

JIRA_FILTER = "E:/study/python/jira/cfg/jira_filter.json"
MAIL_RECEIVER = [
    "jiangpeifu@126.com", 
    "peifu.jiang@amlogic.com"
]
MAIL_SUBJECT = "SECURITY TEAM JIRA LIST Weekly Report"

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
html = HTML_HEAD
filters = get_filters(JIRA_FILTER)
for filter in filters:
    jira_title = "<h4>{}<br></h4>".format(filter["name"])
    jira_table = jc.get_jira_html(filter)
    html += jira_title
    html += jira_table
html += HTML_TAIL
mc.send_mail(receiver, subject, "", html)
