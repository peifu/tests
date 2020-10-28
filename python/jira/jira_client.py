#!/usr/bin/env python3
#coding: utf-8

from jira import JIRA
import re
import json
import prettytable as pt
import pandas as pd
import numpy as np
from io import StringIO

DEBUG_ENABLE = 0

MAX_ISSUE = 50
MAX_SUMMARY = 80
JIRA_SERVER = "E:/study/python/jira/cfg/jira_server.json"
JIRA_FILTER = "E:/study/python/jira/cfg/jira_filter.json"

JIRA_PATTERN = "project in ({}) AND priority in ({}) AND status in ({}) AND assignee in ({}) ORDER BY priority DESC, status ASC, assignee ASC"
FIELD_NAMES = ["Issue", "Issuetype", "Priority", "Status", "Assingee", "Creator", "Summary"]
FIELD_NAMES2 = ["Issue", "Issuetype", "Priority", "Status", "Assingee", "Creator", "Due Date", "Finish Date", "Summary"]

def debug(args):
    if DEBUG_ENABLE == 1:
        print(args)

def store_csv(filename, csv_str):
    csv_file = open("filename", "w+")
    csv_file.write(csv_str)
    csv_file.close()

def init_config(cfg):
    f = open(cfg, encoding="utf-8")
    jira_config = json.load(f)
    f.close()
    debug(jira_config)
    return jira_config

def init_config_with_json(cfg_json):
    jira_config = json.loads(cfg_json)
    debug(jira_config)
    return jira_config

def init_jira(jira_config):
    server = jira_config["server"]["server_addr"]
    user = jira_config["server"]["user"]
    pwd = jira_config["server"]["password"]
    debug("server={}, user={}, pwd={}".format(server, user, pwd))
    jira = JIRA({"server": server}, basic_auth=(user, pwd))
    return jira

def get_filters(cfg):
    f = open(cfg, encoding="utf-8")
    filter_json = json.load(f)
    f.close()
    return filter_json["filter"]

def get_dataframe_from_csv(csv_str):
    df = pd.DataFrame(pd.read_csv(StringIO(csv_str)))
    print(df)
    #pg = df[df['Assingee'].isin(["Pengguang.Zhu", "wei.hong"])]
    #print(pg)

def init_table():
    tb = pt.PrettyTable()
    tb.field_names = FIELD_NAMES2
    tb.align = 'l'
    return tb

def update_table(tb, issue):
    item = [issue.key, issue.fields.issuetype.name, issue.fields.priority.name, 
        issue.fields.status.name, issue.fields.assignee.key, issue.fields.creator.key, 
        issue.fields.summary[0:MAX_SUMMARY]]
    item2 = [issue.key, issue.fields.issuetype.name, issue.fields.priority.name, 
        issue.fields.status.name, issue.fields.assignee.key, issue.fields.creator.key, 
        issue.fields.duedate, issue.fields.customfield_11614,
        issue.fields.summary[0:MAX_SUMMARY]]
    tb.add_row(item2)

def dump_table(tb):
    s = tb.get_string()
    print(s)

def get_csv(tb):
    return tb.get_csv_string()

def dump_issue(issue):
    debug(dir(issue.fields))
    debug('{}:{}'.format(issue.key, issue.fields.summary))
    item = [issue.key, issue.fields.issuetype, issue.fields.priority, issue.fields.status,
         issue.fields.assignee, issue.fields.creator, issue.fields.summary]
    debug(item)
    item2 = [issue.fields.customfield_11614]
    debug(item2)

def query_issues(jira, pattern, count):
    issues = jira.search_issues(pattern, maxResults=count)
    return issues

def get_issues_by_pattern(jira, pattern):
    tb = init_table()
    debug(pattern)
    issues = query_issues(jira, pattern, MAX_ISSUE)
    for issue in issues:
        update_table(tb, issue)
        # debug issue
        dump_issue(issue)
        break
    return tb

def get_issues_by_filter(jira, filter):
    tb = init_table()
    pattern = JIRA_PATTERN.format(filter["project"], filter["priority"], filter["status"], filter["assignee"])
    debug(pattern)
    issues = query_issues(jira, pattern, MAX_ISSUE)
    for issue in issues:
        update_table(tb, issue)
        # debug issue
        dump_issue(issue)
    return tb


def get_jira_csv(filter):
    jira_config = init_config(JIRA_SERVER)
    jira = init_jira(jira_config)
    tb = get_issues_by_filter(jira, filter)
    csv = tb.get_csv_string()
    return csv

def get_jira_html(filter):
    jira_config = init_config(JIRA_SERVER)
    jira = init_jira(jira_config)
    tb = get_issues_by_filter(jira, filter)
    csv = tb.get_csv_string()
    df = pd.DataFrame(pd.read_csv(StringIO(csv)))
    html = df.to_html(index=False)
    return html

def get_jira_table(filter):
    jira_config = init_config(JIRA_SERVER)
    jira = init_jira(jira_config)
    tb = get_issues_by_filter(jira, filter)
    s = tb.get_string()
    return s

def test_jira_table():
    filters = get_filters(JIRA_FILTER)
    for filter in filters[0:1]:
        s = get_jira_table(filter)
        print(filter["name"])
        print(s)

if __name__ == "__main__":
    test_jira_table()