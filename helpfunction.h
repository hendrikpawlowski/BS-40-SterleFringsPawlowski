#define HELPFUNCTION_H

#define true 1
#define false 0


int compare(char *x, char *y)
{
    for (int i = 0;; i++)
    {
        if (x[i] != y[i])
        {
            return false;
        }
        if (x[i] == '\0' && y[i] == '\0')
        {
            return true;
        }
    }
}