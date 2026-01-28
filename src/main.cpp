#include "Application.h"
#include "CanvasLayer.h"
#include "UILayer.h"
#include "AppLayer.h"
#include "DataPersistanceLayer.h"

int main()
{
    Core::Application app{};
    app.push_layer<DataPersitanceLayer>();
    app.push_layer<AppLayer>();
    app.push_layer<CanvasLayer>();
    app.push_layer<UILayer>();
    app.run();
    return 0;
}


/* TODO: 
- wenn man zoomt -> dragen von elementen nicht mehr identisch mit mausposition
    -> transformmation fehlt irgendwo noch
- sollten für die sprite element im canvas layer nicht eher unique_ptr sein, da es die ja owned?

- render() sollte bei uielemente intern bereits codiert sein
    - (warum sollte man selber bei uibutton den render code jedesmal schreiben?)
    - wie ist das mit dem fall bei asset explorer (sind auch buttons + mit mehr rendering)

- asset explorer überarbeiten
    - das "rausdraggen" muss noch richtig implementiert werden
    - sollte klar sein welches gerade fokusiert ist

- wenn loading von scene fehlschlägt -> fehlerhandling (nicht programm crash)

- eigentlich braucht die handle nicht in der scene gespeichert zu werden, da sowie die pfade gespeichert werden

*/