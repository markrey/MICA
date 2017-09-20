import datetime
from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.
def hello(request):
    return HttpResponse('こんにちは')

def card(request):
    return render(request, 'card.html')

def cards(request):
    rank_str = []
    rank = list(range(1,14))
    for x in rank:
        rank_str.append(str(x).zfill(2))
    return render(request, 'cards.html', {'card_rank' : rank_str})

def mqttStart(request):
    return HttpResponse('こんにちは')

def mqtt(request):
    f = open('/tmp/currentList.txt','r')
    msgData = ' '
    for row in f:
        msgData = row.strip()
    f.close()
    mqttData = msgData.split(",")
    now = datetime.datetime.now()

    dictionary = {
        'createTime' : now.strftime("%Y-%m-%dT%H:%M:%SZ"),
        'ID01' : mqttData[0],
        'mistLv01' : mqttData[1],
        'mUnit01' : mqttData[2],
        'temp01'   : mqttData[3],
        'ID02' : mqttData[5],
        'mistLv02' : mqttData[6],
        'mUnit02' : mqttData[7],
        'temp02'   : mqttData[8],
        'ID03' : mqttData[10],
        'mistLv03' : mqttData[11],
        'mUnit03' : mqttData[12],
        'temp03'   : mqttData[13],
        'ID04' : mqttData[15],
        'mistLv04' : mqttData[16],
        'mUnit04' : mqttData[17],
        'temp04'   : mqttData[18],
        'ID05' : mqttData[20],
        'mistLv05' : mqttData[21],
        'mUnit05' : mqttData[22],
        'temp05'   : mqttData[23],
        'ID06' : mqttData[25],
        'mistLv06' : mqttData[26],
        'mUnit06' : mqttData[27],
        'temp06'   : mqttData[28],
        'ID07' : mqttData[30],
        'mistLv07' : mqttData[31],
        'mUnit07' : mqttData[32],
        'temp07'   : mqttData[33],
        'ID08' : mqttData[35],
        'mistLv08' : mqttData[36],
        'mUnit08' : mqttData[37],
        'temp08'   : mqttData[38],
        'ID09' : mqttData[40],
        'mistLv09' : mqttData[41],
        'mUnit09' : mqttData[42],
        'temp09'   : mqttData[43],
        'ID10' : mqttData[45],
        'mistLv10' : mqttData[46],
        'mUnit10' : mqttData[47],
        'temp10'   : mqttData[48],
        'ID11' : mqttData[50],
        'mistLv11' : mqttData[51],
        'mUnit11' : mqttData[52],
        'temp11'   : mqttData[53],
        'ID12' : mqttData[55],
        'mistLv12' : mqttData[56],
        'mUnit12' : mqttData[57],
        'temp12'   : mqttData[58],
        'ID13' : mqttData[60],
        'mistLv13' : mqttData[61],
        'mUnit13' : mqttData[62],
        'temp13'   : mqttData[63],
        'ID14' : mqttData[65],
        'mistLv14' : mqttData[66],
        'mUnit14' : mqttData[67],
        'temp14'   : mqttData[68],
        'ID15' : mqttData[70],
        'mistLv15' : mqttData[71],
        'mUnit15' : mqttData[72],
        'temp15'   : mqttData[73],
        'ID16' : mqttData[75],
        'mistLv16' : mqttData[76],
        'mUnit16' : mqttData[77],
        'temp16'   : mqttData[78],
    }
    return render(request, 'mqttdata.xml', dictionary)

def mtconnect(request):
    return render(request, 'mtconnect.xml')

def current(request):
    return render(request, 'mtconnect.org/current.xml')

def probe(request):
    return render(request, 'mazak/probe.xml')

def assets(request):
    return render(request, 'mazak/assets.xml')

def sample(request):
    return render(request, 'mtconnect.org/sample.xml')

def welcome (request):
    name = '吉野'
    dictionary = {'name' : name }
    return render(request, 'name_if.html', dictionary)

def random_cards(request):
    import random
    suits = ['S', 'H', 'D', 'C']
    ranks = range(1,14)
    deck = [(x,y) for x in ranks for y in suits]
    random.shuffle(deck)

    card1 = deck.pop()
    card2 = deck.pop()

    dictionary = {
        'suit' : card1[1],
        'rank' : str(card1[0]).zfill(2),
        'suit2' : card2[1],
        'rank2' : str(card2[0]).zfill(2),
    }
    return render(request, 'display_cards.html', dictionary)

