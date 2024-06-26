#if !defined(BLANKSCREEN__INCLUDED_)
#define BLANKSCREEN__INCLUDED_

#include "ScreenBase.h"
#include "GuiDevice.h"

#include <FL/Fl.H>

class BlankScreen : public GeomScreen
{
public:
    BlankScreen( ScreenMgr* mgr );
    virtual ~BlankScreen()                          {}

    virtual void Show();
    virtual bool Update();

    virtual void CallBack( Fl_Widget *w );

protected:

};


#endif // !defined(BLANKSCREEN__INCLUDED_)
