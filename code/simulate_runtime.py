import random
from time import sleep
from pathlib import Path

def randomize_value_files(parent_dir_path):
    parent = Path(parent_dir_path)

    for child in parent.iterdir():
        value_file = child / "value"

        new_value = random.choice(['0', '1'])

        value_file.write_text(new_value)

if __name__ == "__main__":
    while True:
        randomize_value_files("./virtual_pins/")
        sleep(5)
