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
    f = open('/tmp/test.txt','r')
    msgData = ' '
    for row in f:
        msgData = row.strip()
    f.close()
    mqttData = msgData.split(",")
    dictionary = {
        'mistLv01' : mqttData[0],
        'temp01'   : mqttData[1],
        'mistLv021' : mqttData[2],
        'temp02'   : mqttData[3],
        'mistLv03' : mqttData[4],
        'temp03'   : mqttData[5],
        'mistLv04' : mqttData[6],
        'temp04'   : mqttData[7],
        'mistLv05' : mqttData[8],
        'temp05'   : mqttData[9],
        'mistLv06' : mqttData[10],
        'temp06'   : mqttData[11],
        'mistLv07' : mqttData[12],
        'temp07'   : mqttData[13],
        'mistLv08' : mqttData[14],
        'temp08'   : mqttData[15],
        'mistLv09' : mqttData[16],
        'temp09'   : mqttData[17],
        'mistLv10' : mqttData[18],
        'temp10'   : mqttData[19],
        'mistLv11' : mqttData[20],
        'temp11'   : mqttData[21],
        'mistLv12' : mqttData[22],
        'temp12'   : mqttData[23],
        'mistLv13' : mqttData[24],
        'temp13'   : mqttData[25],
        'mistLv14' : mqttData[26],
        'temp14'   : mqttData[27],
        'mistLv15' : mqttData[28],
        'temp15'   : mqttData[29],
        'mistLv16' : mqttData[30],
        'temp16'   : mqttData[31],
    }
    return render(request, 'mqttdata.xml', dictionary)

def mtconnect(request):
    return render(request, 'mtconnect.xml')

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

