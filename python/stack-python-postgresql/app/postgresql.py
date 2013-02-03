#!/usr/bin/env python
import json
import psycopg2
from os import path
from time import sleep
from sys import stdout as out
import sys

dotcloud_env_file_path = path.expanduser('~/environment.json')
if path.exists(dotcloud_env_file_path):
    with open(dotcloud_env_file_path) as f:
        env = json.load(f)
else:
        env = {
        'DOTCLOUD_DB_SQL_LOGIN': 'root',
        'DOTCLOUD_DB_SQL_PASSWORD': 'root',
        'DOTCLOUD_DB_SQL_HOST': 'localhost',
        'DOTCLOUD_DB_SQL_PORT': '5432',
    }

dbname = "test"

out.write("Creating the database...")
out.flush()
i = 120
while True:
    try:
        conn = psycopg2.connect(
                                user=env['DOTCLOUD_DB_SQL_LOGIN'],
                                password=env['DOTCLOUD_DB_SQL_PASSWORD'],
                                host=env['DOTCLOUD_DB_SQL_HOST'],
                                port=env['DOTCLOUD_DB_SQL_PORT'],
                                database='postgres')
        conn.set_isolation_level(0)
        cur = conn.cursor()
        cur.execute("CREATE DATABASE %s" % dbname)
        out.write("\n")
        out.flush()
        break
    except Exception as e:
        i -= 1
        if i <= 0 :
            sys.exit(1)
        sleep(2)
        out.write(".")
        out.flush()

if __name__ == '__main__':
    import sys