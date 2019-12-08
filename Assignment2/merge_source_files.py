import os

out = ''
     
for root, dirs, files in os.walk('./'):
    for f_name in filter(lambda x: any(x.endswith(end) for end in['.h','.cpp','.ino']), files):
        with open(f_name) as f:
            out += "\\\\ " + f_name + ' file\n\n' + f.read() + '\n\n\n\n\n\n'

with open('merged_source_code.txt', 'w') as f:
    f.write(out)
