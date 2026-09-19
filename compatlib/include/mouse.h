#ifndef POLANIE_MOUSE_H
#define POLANIE_MOUSE_H

/* Although this is called Mouse, it also handles keyboard input */
class Mouse
{
public:
    Mouse();
    int ClickCount(int p_button);
    int IsInputReady();
    int IsInBoundary(int p_x1, int p_y1, int p_x2, int p_y2);

    void SetKeyReady(int p_keyReady);
    void IncrementPresses(int p_button);
    void ClearPresses();

    int X;
    int Y;
    int oldX;
    int oldY;
    int Button;
    int Key;

private:
    int m_keyReady;
    int m_leftPresses;
    int m_rightPresses;
};

extern Mouse mouse;

#endif //POLANIE_MOUSE_H
