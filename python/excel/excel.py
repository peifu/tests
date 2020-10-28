#!/usr/bin/env python
# -*- coding: utf-8 -*-

import xlrd
import xlwt
import datetime
import os

class excelProcess:
    def __init__(self,keywordExcelFile,mainExcelFile):
        self.keywordExcelFile = keywordExcelFile
        self.mainExcelFile = mainExcelFile

    def WriteLog(self, message,date):
        fileName = os.path.join(os.getcwd(),  date  +   '.txt')
        with open(fileName, 'a') as f:
            f.write(message)

    def WriteSheetRow(self,sheet, rowValueList, rowIndex, isBold):
        i = 0
        style = xlwt.easyxf('font: bold 1')
        # style = xlwt.easyxf('font: bold 0, color red;')
        # style2 = xlwt.easyxf('pattern: pattern solid, fore_colour yellow; font: bold on;')
        for svalue in rowValueList:
            if isBold:
                sheet.write(rowIndex, i, svalue, style)
            else:
                sheet.write(rowIndex, i, svalue)
            i = i + 1

    def save_Excel(self):
        wbk = xlwt.Workbook()
        sheet = wbk.add_sheet('sheet1', cell_overwrite_ok=True)
        headList = ['IR_SITENAME', 'IR_AUTHORS', 'SY_INFOTYPE', 'RID', 'IR_URLTITLE','SY_KEYWORDS',
                    'IR_URLNAME', 'IR_URLTIME',
                    'IR_GROUPNAME', 'IR_CHANNEL',
                    'SY_BB_COMMON', 'summary', 'keyword'
                    ]

        rowIndex = 0
        self.WriteSheetRow(sheet, headList, rowIndex, True)
        for i in range(1, 11):
            rowIndex = rowIndex + 1
            valueList = []
            for j in range(1, 14):
                valueList.append(j * i)
            self.WriteSheetRow(sheet, valueList, rowIndex, False)
        fileName = os.path.join(os.getcwd(),'test.xlsx')
        wbk.save(fileName)
