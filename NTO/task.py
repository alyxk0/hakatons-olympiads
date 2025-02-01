import numpy as np
from scipy.signal import welch
import matplotlib.pyplot as plt

filename = '27_2_2024_16_41_17.dat'
filename1 = '27_2_2024_16_43_43.dat'

with open(filename, 'r') as file:
    lines = file.readlines()
    eeg_data = np.array([list(map(float, line.split())) for line in lines])
with open(filename1, 'r') as file:
    lines1 = file.readlines()
    eeg_data1 = np.array([list(map(float, line1.split())) for line1 in lines1])

#мы определяем время каждой записи находя количество интервалов используя последний канал счётчика
#мы определили, что каждые 0-255 счётчика соответствуют 0,76 секунды(см скриншот "ДАТЧИК"), значит чтобы найти время измерения,
#нам нужно найти количество интервалов 0-255 в измерениях и умножить их на 0,76 секунды
t = len(lines)/256 * 0.76
t1 = len(lines1)/256 * 0.76

sampling_rate = len(eeg_data) / t  #частота в гц для закрытых глаз
n_samples = eeg_data.shape[0]
sampling_rate1 = len(eeg_data1) / t1  #частота в гц для закрытых глаз и задачи
n_samples1 = eeg_data1.shape[0]

#границы ритмов в гц
rhythms = {'Дельта': (0.5, 4), 'Тета': (4, 8), 'Альфа': (8, 13), 'Бета': (13, 30), 'Гамма': (30, sampling_rate / 2)}
rhythms1 = {'Дельта': (0.5, 4), 'Тета': (4, 8), 'Альфа': (8, 13), 'Бета': (13, 30), 'Гамма': (30, sampling_rate1 / 2)}


print("Глаза закрыты:")
for i in range(eeg_data.shape[1]):
    channel_data = eeg_data[:, i]

    #используем метод Велча для оценки мощности
    freqs, psd = welch(channel_data, fs=sampling_rate)

    print(f"Канал {i + 1}:")
    for rhythm_name, (low_freq, high_freq) in rhythms.items():
        idx = np.logical_and(freqs >= low_freq, freqs <= high_freq)
        rhythm_power = np.sum(psd[idx])
        #делим полученную мощность на количество измерений, чтобы мощность была нормализована относительно времени
        print(f"Мощность {rhythm_name}: {rhythm_power/len(lines)*1000:.2f}")


print("Глаза закрыты, но решаем задачу:")
for i in range(eeg_data1.shape[1]):
    channel_data1 = eeg_data1[:, i]

    #используем метод Велча для оценки спектральной плотности мощности
    freqs1, psd1 = welch(channel_data1, fs=sampling_rate1)

    print(f"Канал {i + 1}:")
    for rhythm_name1, (low_freq1, high_freq1) in rhythms1.items():
        idx = np.logical_and(freqs1 >= low_freq1, freqs1 <= high_freq1)
        rhythm_power = np.sum(psd1[idx])
        #делим полученную мощность на количество измерений, чтобы мощность была нормализована относительно времени
        print(f"Мощность {rhythm_name1}: {rhythm_power/len(lines1)*1000:.2f}")


#здесь мы проверили сходство графика и полученного значения мощности на глаз, просто чтобы убедиться
#start_index = 100
#sequential_samples = eeg_data[start_index:start_index + 500, 3]

#plt.figure(figsize=(12, 6))
#plt.plot(sequential_samples)
#plt.title('Последовательные выборки сигнала ЭЭГ 8-го канала')
#plt.xlabel('Время')
#plt.ylabel('Амплитуда')
#plt.show()
