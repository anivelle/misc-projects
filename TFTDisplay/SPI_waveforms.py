import cairo

DELAY = 0x80
LINE_HEIGHT = 80
WIDTH = 800
HEIGHT = 300
PADDING = 20

def print_wire(context: cairo.Context, data, is_cmd, data_width):
    def draw_line(x, y, x1, y1):
        context.move_to(x, y)
        context.line_to(x1, y1)
        context.stroke()
    
    x, px = PADDING, PADDING
    start_height = (HEIGHT - 25) / 3
    sday, psday =  start_height, start_height
    # SCL starts high
    scly, pscly =  2 * sday - LINE_HEIGHT,  2 * psday - LINE_HEIGHT
    csy,  pcsy =  3 * sday,  3 * psday

    if (is_cmd):
        data |= 0x100
    x += data_width
    draw_line(px, pcsy, x, csy)
    draw_line(px, psday, x, sday)
    draw_line(px, pscly, x, scly)
    px = x

    # Start SCL
    scly += LINE_HEIGHT


    # Assert CS
    csy -= LINE_HEIGHT
    draw_line(px, pcsy, x, csy)
    pcsy = csy

    print("New command")
    data = data & 0xff
    for _ in range(9):
        print(data)
        print(data & 0x100)
        if data & 0x100:
            if sday == start_height:
                sday -= LINE_HEIGHT
            draw_line(px, psday, x, sday)
            draw_line(px, pscly, x, scly)
            pscly = scly

        else:
            if sday == start_height - LINE_HEIGHT:
                sday += LINE_HEIGHT
            draw_line(px, psday, x, sday)
            draw_line(px, pscly, x, scly)

        pscly = scly
        psday = sday
            
        # Advance the "time"
        x += data_width
        draw_line(px, psday, x, sday)
        draw_line(px, pcsy, x, csy)   
        draw_line(px, pscly, x, scly)
        px = x
        psday = sday

        scly -= LINE_HEIGHT

        draw_line(px, pscly, x, scly)
        pscly = scly

        x += data_width
        draw_line(px, psday, x, sday)
        draw_line(px, pcsy, x, csy)   
        draw_line(px, pscly, x, scly)
        px = x
        pscly = scly

        scly += LINE_HEIGHT
        data = (data << 1) & 0x1ff


if __name__ == "__main__":
    with open("./main/init_vars.h", 'r') as init_vars:
        with cairo.SVGSurface("waveforms.svg", WIDTH, HEIGHT) as surface:
            context = cairo.Context(surface)
            saved = 0
            while init_vars.readline().find("lcd_init") == -1:
                pass
            for line in init_vars:
                context.set_source_rgb(0, 0, 0)
                context.paint()
                context.set_source_rgb(0.75, 0.8, 0.3)
                context.set_line_width(0.5)

                line = line.strip(' \n,')
                if line[0] == '' or line[0] == '}':
                    break
                cmd_bytes = [int(cmd.strip(' '), 16) for cmd in line.split(', ')]
                command = cmd_bytes[0]
                data_size = cmd_bytes[1]
                data = cmd_bytes[2:]
                delay = data_size & DELAY != 0
                if delay:
                    data_size = 1
                byte_width = (WIDTH - 2 * PADDING) / (9.5 * data_size)
                print_wire(context, command, True, byte_width)
                if saved == 2:
                    surface.write_to_png("Test.png")
                    
                context.set_source_rgb(0, 0, 0)
                context.paint()
                saved += 1
