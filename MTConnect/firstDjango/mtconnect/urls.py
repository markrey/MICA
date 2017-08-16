from django.conf.urls import url
from . import views

urlpatterns = [
    url(r'^hello/$', views.hello),
    url(r'^card/$', views.card),
    url(r'^mtconnect/$', views.mtconnect),
    url(r'^welcome/$', views.welcome),
    url(r'^cards/$', views.cards),
    url(r'^random_cards/$', views.random_cards),
    url(r'^mqtt/$', views.mqtt),
    url(r'^mqttStart/$', views.mqttStart),
]

