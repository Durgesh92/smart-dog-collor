#  Smart Dog Collor

##  Features
- Supports Custom Pet Name
- Wakes on detection of pet name
- Understands and responds for predefined questions
- Totally offline on device


##  Setting up device

- Install ubuntu desktop 21.04 in pi (I have worked on 3b but same procedure for 4 as well)

	![alt text](https://github.com/Durgesh92/smart-dog-collor/blob/master/1.png)

- linux dependencies
```bash
	sudo apt install lame sox ffmpeg
```

- install python dependencies (make sure to use python3.8 as default python)

```bash
pip install virtualenv
virtualenv -p python3 pyenv
source pyenv/bin/activate
pip install numpy scipy pydub sounddevice nltk
```
- Install library
```bash
cd python_durgesh.ai
python setup.py install
```

- To start the recognizer
```bash
python test_wake.py
```
