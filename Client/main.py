#!/usr/bin/env python
# -*- coding: utf-8 -*-
# https://github.com/python-telegram-bot/python-telegram-bot/wiki/Code-snippets
# https://github.com/python-telegram-bot/python-telegram-bot/wiki/Code-snippets#custom-keyboards
# https://github.com/eternnoir/pyTelegramBotAPI#methods
# https://core.telegram.org/bots/api
import os
import telebot
import json
import time
from datetime import datetime
from telebot import types


TOKEN = '379347283:AAFbH-L81FTO5z-Py4Teiq3m4yOZMNc50ys' # Ponemos nuestro Token generado con el
bot = telebot.TeleBot(TOKEN)
def CreaArchivo():
    archivo1=open('post.json','a')

class Message:
	def __init__(self):
		self.flag = True
		pass

	def keyboard(self):
		markup = types.ReplyKeyboardMarkup(row_width=2)
		keyboard = [
			types.KeyboardButton('ALERTA DE INCENDIO', request_location=True),
			types.KeyboardButton('FALSA ALERTA DE INCENDIO', request_location=True),
			types.KeyboardButton('CONSULTAR CERCANIA DE INCENDIO', request_location=True)
		]
		markup.add(keyboard[0], keyboard[1], keyboard[2])
		return markup

	def keyboard_hide(self):
		markup = types.ReplyKeyboardRemove(selective=False)
		return markup

	def inline_keyboard(self):
		markup = types.InlineKeyboardMarkup(row_width=2)
		keyboard = [
			types.InlineKeyboardButton('ALERTA DE INCENDIO'),
			types.InlineKeyboardButton('FALSA ALERTA DE INCENDIO'),
			types.InlineKeyboardButton('CONSULTAR CERCANIA DE INCENDIO')
		]
		markup.add(keyboard[0], keyboard[1], keyboard[2])
		return markup

	def send(self, chat_id, message, markup):
		bot.send_message(chat_id, message, reply_markup=markup)
		self.flag = True

	def welcome(self, m):
		bot.send_message(m.chat.id, "¡Bienvenido!")
		bot.send_message(m.chat.id, "Será un agrado recibir su información")
		bot.send_message(m.chat.id, "Espero que nos llevémos muy bien :)")

	def location(self, m):
		print ("({0}, {1})".format(m.location.latitude, m.location.longitude)),
		print (datetime.fromtimestamp(m.date))
		timestamp=time.time()
		print (timestamp)
		bot.send_message(m.chat.id, "Muchas gracias por su información", reply_markup=self.keyboard())
		diccionario={'latitude':m.location.latitude,
		'longitude':m.location.longitude,
		'timestamp':timestamp}
		print (diccionario)


msn = Message()

@bot.message_handler(commands=['start', 'help'])
def send_welcome(message):
	msn.welcome(message)

@bot.message_handler(content_types=['location'])
def handle_location(message):
	msn.location(message)

@bot.message_handler(func=lambda m: True)
def echo_all(m):
	if m.text == "ALERTA DE INCENDIO":
		print (1)
	if m.text == "FALSA ALERTA DE INCENDIO":
		print (2)
	if m.text == "CONSULTAR CERCANIA DE INCENDIO":
		print (3)
	bot.send_message(m.chat.id, "Envíenos una ubicación", reply_markup=msn.keyboard())
bot.polling()
