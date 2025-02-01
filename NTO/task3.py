from psychopy import visual, core, event
import random

win = visual.Window([800, 600], color="grey", units="pix")

text = "Я - скрипт, и я вывожу на экран буквы"
initial_color = "white"

letters = [visual.TextStim(win, text=char, pos=(i*15 - len(text.replace(" ", ""))*7.5, 0), color=initial_color) for i, char in enumerate(text) if char != " "]
spaces = [i for i, char in enumerate(text) if char == " "] 
indicator = visual.Rect(win, width=50, height=50, pos=[300, -200], fillColor="black", lineColor="black")

key_pressed = False
change_position = False

while True:
    x_offset = 0  
    for index, letter in enumerate(letters):
        while index + x_offset in spaces:  
            x_offset += 1
        letter.pos = ((index + x_offset)*15 - len(text.replace(" ", ""))*7.5, 0)
        letter.draw()
    indicator.draw()
    win.flip()

    keys = event.getKeys()

    if 'escape' in keys:
        break

    if keys and not key_pressed:
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
        core.wait(0.5)
        random_letter.color = initial_color
        random_letter.pos = original_pos
        indicator.fillColor = "black"
        for letter in letters:
            letter.draw()
        indicator.draw()
        win.flip()
        core.wait(0.5)
        

win.close()
