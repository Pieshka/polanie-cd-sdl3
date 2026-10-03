#include "mouse.h"
#include <SDL3/SDL_mouse.h>

Mouse mouse;

Mouse::Mouse()
{
    X = 0;
    Y = 0;
    oldX = 0;
    oldY = 0;
    Key = 0;
    Button = 0;

    m_keyReady = 0;
    m_rightPresses = 0;
    m_leftPresses = 0;

    m_gamepadSpeed = 5;
}

int Mouse::ClickCount(int p_button)
{
    int tempPresses;
    if (p_button == 0)
    {
        tempPresses = m_leftPresses;
        m_leftPresses = 0;
        return tempPresses;
    }
    if (p_button == 1)
    {
        tempPresses = m_rightPresses;
        m_rightPresses = 0;
        return tempPresses;
    }
    return 0;
}

int Mouse::IsInputReady()
{
    if (m_keyReady)
    {
        m_keyReady = 0;
        return 1;
    }

    Key = 0;
    if (!Button) Button = ClickCount(1);
    return Button;
}

int Mouse::IsInBoundary(int p_x1, int p_y1, int p_x2, int p_y2)
{
    if (Button && X >= p_x1 && X <= p_x2 && Y >= p_y1 && Y <= p_y2)
        return 1;

    return 0;
}

int Mouse::GetGamepadSpeed()
{
    return m_gamepadSpeed;
}

int Mouse::GetIsInMotion()
{
    return m_isInMotion;
}

int Mouse::GetAxisValue(int p_axis)
{
    if (p_axis == 0)
        return m_axisXValue;

    return m_axisYValue;
}

void Mouse::SetKeyReady(int p_keyReady)
{
    m_keyReady = p_keyReady;
}

void Mouse::IncrementPresses(int p_button)
{
    if (p_button == SDL_BUTTON_LEFT) { m_leftPresses++; }
    if (p_button == SDL_BUTTON_RIGHT) { m_rightPresses++; }
}

void Mouse::ClearPresses()
{
    m_leftPresses = m_rightPresses = 0;
}

void Mouse::SetIsInMotion(int p_isInMotion)
{
    m_isInMotion = p_isInMotion;
}

void Mouse::SetAxisValue(int p_axis, int p_value)
{
    if (p_axis == 0)
    {
        m_axisXValue = p_value;
    }

    if (p_axis == 1)
    {
        m_axisYValue = p_value;
    }
}

void Mouse::IncreaseGamepadSpeed()
{
    m_gamepadSpeed++;
}

void Mouse::DecreaseGamepadSpeed()
{
    m_gamepadSpeed = SDL_max(0, m_gamepadSpeed-1);
}
