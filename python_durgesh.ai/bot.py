import nltk
from nltk.chat.util import Chat, reflections
import hashlib
import subprocess

rules = []

valid_hash = []

with open("out_hash.csv","r") as f1:
        for line in f1:
                ls = line.strip().split("\t")
                valid_hash.append(ls[0])
                tmp_ = []
                tmp_.append(r""+ls[1]+"")
                tmp__ = []
                for x in ls[2:]:
                        tmp__.append(x)
                tmp_.append(tmp__)
                rules.append(tmp_)

def chat(inp):
    result = hashlib.sha1(inp.encode()).hexdigest()
    if result in  valid_hash:
        chat = Chat(rules, reflections)
        res = chat.respond(inp)
        play_response(result,hashlib.sha1(res.encode()).hexdigest())

def play_response(que_hash,ans_hash):
    au_ = "audio/"+que_hash+"/"+ans_hash+".mp3"
    cmd = "lame --decode "+au_+" - | play -"
    ps = subprocess.Popen(('lame', '--decode',au_,'-'), stdout=subprocess.PIPE)
    output = subprocess.check_output(('play', '-'), stdin=ps.stdout)
    ps.wait()
	
#initiate the conversation
if __name__ == "__main__":
    chat("are you happy")
