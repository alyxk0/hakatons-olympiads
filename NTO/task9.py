import random
import serial
import threading
import csv
import matplotlib.pyplot as plt
from psychopy import visual, core, event

ser = serial.Serial('COM6', 115200)
win = visual.Window([800, 600], color="grey", units="pix")

text = list("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ<>=")
initial_color = "white"

# Создаем таблицу 6x6
table = [[' ' for _ in range(6)] for _ in range(6)]
for i in range(36):
    table[i // 6][i % 6] = text[i]

letters = [visual.TextStim(win, text=char, pos=((i - 2.5) * 80, (j - 2.5) * 80), color=initial_color) for j in range(6) for i in range(6) for char in table[j][i] if char != " "]
spaces = [(i, j) for j in range(6) for i in range(6) if table[j][i] == " "]
indicator = visual.Rect(win, width=50, height=50, pos=[300, -200], fillColor="black", lineColor="black")

int_values = []
all_int_values = []  #Создаем глобальный массив для накопления данных
key_pressed = False
change_position = True

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

mig_row = []
mig_col = []
while True:
    x_offset = 0
    y_offset = 0
    for j in range(6):
        for i in range(6):
            while (i + x_offset, j + y_offset) in spaces:
                x_offset += 1
            letter = letters[(j + y_offset) * 6 + (i + x_offset)]
            letter.pos = ((i + x_offset - 2.5) * 80, (j + y_offset - 2.5) * 80)
            letter.draw()
        x_offset = 0
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
        if change_position:
            # Выбираем случайную строку или столбец для замены
            row_or_col = random.choice(['row', 'col'])
            if row_or_col == 'row':
                random_row = random.choice(range(6))
                mig_row.append(random_row)
                mig_col.append(-1)
                print(mig_row)
                print(mig_col)
                original_color = [letter.color for letter in letters if letter.pos[1] == (random_row - 2.5) * 80]
                letter_arr = []
                for letter in letters:
                    if letter.pos[1] == (random_row - 2.5) * 80:
                        letter.color = "red"
                        letter_arr.append(letter)
                indicator.fillColor = "white"
                for letter in letters:
                    letter.draw()
                indicator.draw()
                win.flip()
                core.wait(0.75)
                for letter in letter_arr:
                    letter.color = "white"
                indicator.fillColor = "black"
                for letter in letters:
                    letter.draw()
                indicator.draw()
                cntsq += 1
                win.flip()
                core.wait(0.75)
            else:
                random_col = random.choice(range(6))
                mig_row.append(-1)
                mig_col.append(random_col)
                print(mig_row)
                print(mig_col)
                original_color = [letter.color for letter in letters if letter.pos[0] == (random_col - 2.5) * 80]
                letter_arr = []
                for letter in letters:
                    if letter.pos[0] == (random_col - 2.5) * 80:
                        letter.color = "red"
                        letter_arr.append(letter)
                indicator.fillColor = "white"
                for letter in letters:
                    letter.draw()
                indicator.draw()
                win.flip()
                core.wait(0.75)
                for letter in letter_arr:
                    letter.color = "white"
                indicator.fillColor = "black"
                for letter in letters:
                    letter.draw()
                indicator.draw()
                cntsq += 1
                win.flip()
                core.wait(0.75)

with open('zad9.csv', 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile, delimiter=';')
    for i in range(0, len(all_int_values), 10):
        csvwriter.writerow(all_int_values[i:i+10])

with open('mig_row.csv', 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile, delimiter=';')
    for value in mig_row:
        csvwriter.writerow([value])

# Запись значений mig_col в файл mig_col.csv
with open('mig_col.csv', 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile, delimiter=';')
    for value in mig_col:
        csvwriter.writerow([value])


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
