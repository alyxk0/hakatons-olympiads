from psychopy import visual, core, event
import random
import serial
import threading
import csv
import matplotlib.pyplot as plt

ser = serial.Serial('COM6', 115200)
win = visual.Window([800, 600], color="grey", units="pix")

text = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ<>="
initial_color = "white"

letters = [visual.TextStim(win, text=char, pos=(i * 15 - len(text.replace(" ", "")) * 7.5, 0), color=initial_color) for i, char in enumerate(text) if char != " "]
spaces = [i for i, char in enumerate(text) if char == " "]
indicator = visual.Rect(win, width=50, height=50, pos=[300, -200], fillColor="black", lineColor="black")

int_values = []
all_int_values = []  #Создаем глобальный массив для накопления данных
key_pressed = False
change_position = False

countar = []
all_int_values_8 = []
def read_serial():
    tr = False
    global int_values
    while True:
        if ser.in_waiting > 0:  #Проверяем, есть ли данные для чтения
            line = ser.readline()
            decoded_str = line.decode('utf-8').strip()
            values = decoded_str.split(',')
            int_values = [int(value) for value in values]
            all_int_values.extend(int_values)
            if len(int_values) > 8:
                if (int_values[8] < 15):
                    all_int_values_8.append(0)
                else:
                    all_int_values_8.append(int_values[8])
            if int_values[8] > 60 and tr == False:
                countar.append(1)
                tr = True
            elif int_values[8] <= 60:
                tr = False

#Запускаем чтение данных из Serial порта в отдельном потоке
thread = threading.Thread(target=read_serial)
thread.daemon = True  #Устанавливаем поток как демон, чтобы он завершился при закрытии основной программы
thread.start()
cntsq=0

while True:
    x_offset = 0
    for index, letter in enumerate(letters):
        while index + x_offset in spaces:
            x_offset += 1
        letter.pos = ((index + x_offset) * 15 - len(text.replace(" ", "")) * 7.5, 0)
        letter.draw()
    indicator.draw()
    win.flip()

    keys = event.getKeys()

    if 'escape' in keys:
        break

    if 'space' in keys:
        key_pressed = True

    elif 'space' in keys and key_pressed:
        change_position = not change_position

    if key_pressed:
        random_letter = random.choice(letters)
        original_pos = random_letter.pos
        if change_position:
            random_letter.pos = (random.uniform(-300, 300), random.uniform(-200, 200))
        random_letter.color = "red"
        indicator.fillColor = "white"
        for letter in letters:
            letter.draw()
        indicator.draw()
        win.flip()
        core.wait(0.75)
        random_letter.color = initial_color
        random_letter.pos = original_pos
        indicator.fillColor = "black"
        for letter in letters:
            letter.draw()
        indicator.draw()
        cntsq += 1
        win.flip()
        core.wait(0.75)

with open('output.csv', 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile, delimiter=';')
    csvwriter.writerow(all_int_values)

win.close()
ser.close()

print("")
print("")
print("")

print("Данные успешно сохранены в output.csv")
print("Посчитаны в результате работы светодиода(светодиод фиксирует цвет квадрата):")
print(len(countar))
print("Посчитаны по включению белого цвета квадрата в программе(просто счётчик в программе):")
print(cntsq)

plt.figure(figsize=(10, 5))
plt.plot(all_int_values_8, linestyle='-', color='b')
plt.title('График значений int_values[8] из Serial порта')
plt.xlabel('Порядковый номер измерения')
plt.ylabel('Значение int_values[8]')
plt.grid(True)
plt.show()

