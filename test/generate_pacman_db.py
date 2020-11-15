#!/usr/bin/python

import io
import json
import os
import tarfile


def generate_desc(pkg):
    desc = ""

    for key, value in pkg.items():
        if not isinstance(value, list):
            value = [value]

        if not value:
            continue

        desc += f"%{key.upper()}%\n"

        for val in value:
            desc += f"{val}\n\n"

    return desc


def generate_files(pkg):
    files = ""

    pkgfiles = ['etc/', 'etc/pacman.conf', 'var/', 'var/lib/', 'var/lib/pacman']
    backup = ['etc/pacman.conf 9dd54ec11513b2838bffb04dab66c50a']

    files += "%FILES%\n"
    for f in pkgfiles:
        files += f'{f}\n'

    files += '\n'

    files += "%BACKUP%\n"
    for f in backup:
        files += f'{f}\n'

    return files


def get_db_data():
    curpath = os.path.dirname(os.path.realpath(__file__))
    return json.load(open(f'{curpath}/db.json'))


def generate_syncdb(dbfile):
    pkgs = get_db_data()

    with tarfile.open(dbfile, "w") as tar:
        for pkg in pkgs:
            data = generate_desc(pkg)
            info = tarfile.TarInfo(f"{pkg['name']}-{pkg['version']}/desc")
            info.size = len(data)
            tar.addfile(info, io.BytesIO(data.encode()))


def generate_localdb(dbloc):
    pkgs = get_db_data()

    for pkg in pkgs[:1]:
        path = f"{dbloc}/{pkg['name']}-{pkg['version']}"
        os.makedirs(path)

        with open(f'{path}/desc', 'w') as f:
            f.write(generate_desc(pkg))

        with open(f'{path}/files', 'w') as f:
            f.write(generate_files(pkg))


def main():
    generate_syncdb('/tmp/core.db')
    generate_localdb('/tmp/local/')
    print('generated pacman db in /tmp/')


if __name__ == "__main__":
    main()
