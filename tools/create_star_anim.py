startpos = [-150, -800, 0]
endpos = [-900, 800, 0] # silver star
# endpos = [-150, 800, 0] # red star

byframe = 45

NUM_FRAMES = 164

intermediate_positions = [startpos]

def approach_f32_asymptotic(current: float, target: float, multiplier: float):
    if (multiplier > 1.0): multiplier = 1.0

    return current + ((target - current) * multiplier)

MULTIPLIER = 0.2

for i in range(byframe):
    intermediate_positions.append([
        approach_f32_asymptotic(intermediate_positions[-1][0], endpos[0], MULTIPLIER),
        approach_f32_asymptotic(intermediate_positions[-1][1], endpos[1], MULTIPLIER),
        approach_f32_asymptotic(intermediate_positions[-1][2], endpos[2], MULTIPLIER)
    ])

for i in range(NUM_FRAMES - byframe):
    intermediate_positions.append(endpos)

i = 0
while i < NUM_FRAMES:
    pos = intermediate_positions[i];
    print(f"{{0, 0, 0, {pos[0]:.2f}, {pos[1]}, {pos[2]}}}, ", end='')
    if i != 0 and i % 3 == 0:
        print()
        print("    ", end='')
    i += 1


