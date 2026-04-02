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
- canvas 
    - strg+z einbauen
    - ppu sollte nichtmehr hardcoded sein
    - scene elemente adden sollte über eine zentrale funktion laufen
    - scene elemente focus? sollte über eine zentrale funktion laufen

- asset explorer

- hierachy
    - gibt komischen bugg wo ich ein neuen element aus asset explorer ziehe und es hat eine rotation?

- function ribbon
    - möglichkeit screen reference auszuwählen
    - project öffnen -> path eingeben 
    - neues project erstellen

- misc
    - ppu macht nicht wirklich sinn
    - assets müssen gesaved werden
    - pfad soll in scene save files gespeichert werden?
    - eigentlich müssten hierachy scrollview und assetexplorer scrollen über die selbe basis laufen -> refactor

- UIComponent -> sollte eigentlich alles const sein -> nichts ändern beim rendern!!

- sollten für die sprite element im canvas layer nicht eher unique_ptr sein, da es die ja owned?

- render() sollte bei uielemente intern bereits codiert sein
    - (warum sollte man selber bei uibutton den render code jedesmal schreiben?)
    - wie ist das mit dem fall bei asset explorer (sind auch buttons + mit mehr rendering)

- wenn loading von scene fehlschlägt -> fehlerhandling (nicht programm crash)

- eigentlich braucht die handle nicht in der scene gespeichert zu werden, da sowie die pfade gespeichert werden

- gibt es irgendeinen grudn warum pointer bei m_sprite_elemnts benutzt wird?

*/