def bezier(p0, p1, p2, p3, t):
    return p0*(1-t)**3 + 3*p1*t*((1-t)**2) + 3*p2*(t**2)*(1-t) + p3*t**3
               
p0 = (10, 100)
p1 = (10, 400)
p2 = (400, 400)
p3 = (400, 10)

pygame.display.set_caption("Bezier Demo")
while do_continue():   
    for event in pygame.event.get():
        if event.type == pygame.QUIT: sys.exit()
        elif event.type == MOUSEBUTTONDOWN:
            pressed = pygame.key.get_pressed()
            if event.button == 1:
                if pressed[K_SPACE]:
                    p0 = event.pos
                else:
                    p1 = event.pos
            elif event.button == 3:
                if pressed[K_SPACE]:
                    p3 = event.pos
                else:
                    p2 = event.pos

    screen.fill((0,0,0))
    pygame.draw.line(screen, (255, 0, 255), p0, p1, 3)
    pygame.draw.line(screen, (0, 255, 0), p2, p3, 3)

    last = None
    for i in range(0, 101, 5):
        t = i/100.0
        bx = bezier(p0[0], p1[0], p2[0], p3[0], t)
        by = bezier(p0[1], p1[1], p2[1], p3[1], t)

        if last:
            pygame.draw.line(screen, (255, 255, 255), last, (bx, by), 3)
        last = (bx,by)

    pygame.time.wait(100) 
    pygame.display.flip()

# EOF #
