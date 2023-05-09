        lw      0       2       mcand
        lw      0       3       mplier
        lw      0       4       number
        lw      0       5       r
        lw      0       6       count
        lw      0       7       amo
        noop
loop    beq     4       7       done
        nor     1       3       6
        add     1       1       1
        beq     0       1       right
        add     5       5       2
right   add     2       2       2
        add     4       4       6
        add     3       3       3
        beq     0       0       loop
done    add     1       3       0
        halt
mcand   .fill   32766
mplier  .fill   12328
number  .fill   0
r       .fill   0
count   .fill   1
amo     .fill   32768
