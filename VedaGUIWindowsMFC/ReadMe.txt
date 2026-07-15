=================================================================================================
    BIBLIOTHÈQUE MICROSOFT FOUNDATION CLASS : Vue d'ensemble du projet VedaDemoOGLMfcGui
=================================================================================================

AppWizard a créé cette application VedaDemoOGLMfcGui à votre attention. Cette application
explique les principes fondamentaux de l'utilisation des classes MFC (Microsoft Foundation Class)
et constitue également un point de départ pour écrire votre propre application.

Ce fichier fait la synthèse des différents éléments contenus dans chacun des fichiers qui constituent
votre application VedaDemoOGLMfcGui.

VedaDemoOGLMfcGui.vcproj
    Il s'agit du fichier projet principal pour les projets VC++ générés à l'aide d'un Assistant Application.
    Ce fichier contient des informations sur la version de Visual C++ qui a été utilisée pour générer
    le fichier ainsi que des informations relatives aux plates-formes, configurations et fonctionnalités
    sélectionnées dans l'Assistant Application.

VedaDemoOGLMfcGui.h
    Il s'agit du fichier d'en-tête principal pour l'application. Il comporte
    d'autres en-têtes de projet spécifiques (notamment Resource.h) et déclare la
    classe d'application CVedaDemoOGLMfcGuiApp.

VedaDemoOGLMfcGui.cpp
    Il s'agit du fichier source principal pour l'application. Il contient la classe
    d'application CVedaDemoOGLMfcGuiApp.

VedaDemoOGLMfcGui.rc
    Ce fichier dresse la liste de toutes les ressources Microsoft Windows utilisées
    par le programme, telles que les icônes, bitmaps et curseurs qui sont stockés
    dans le sous-répertoire RES. Vous pouvez modifier ce fichier directement dans Microsoft
    Visual C++. Vos ressources projet se trouvent dans 1036.

res\VedaDemoOGLMfcGui.ico
    Il s'agit du fichier icône utilisé en tant qu'icône de l'application. Cette icône
    est incluse dans le fichier de ressources principal VedaDemoOGLMfcGui.rc.

res\VedaDemoOGLMfcGui.rc2
    Ce fichier contient les ressources qui ne sont pas modifiées par Microsoft 
    Visual C++. Vous devez y placer toutes les ressources non modifiables par
    l'Éditeur de ressources.
VedaDemoOGLMfcGui.reg
    C'est un exemple de fichier .REG qui vous montre quels paramètres d'inscription
    sont définis par l'infrastructure. Vous pouvez l'utiliser comme un fichier .REG
    avec votre application ou bien le supprimer afin que les paramètres d'inscription
    définis dans RegisterShellFileTypes soient appliqués par défaut.
/////////////////////////////////////////////////////////////////////////////

Pour la fenêtre frame principale :
    Style Explorateur Windows : Le projet a une interface de type Explorateur Windows,
    avec deux frames.
MainFrm.h, MainFrm.cpp
    Ces fichiers contiennent la classe de frames CMainFrame, dérivée de
    CFrameWnd et qui contrôle toutes les fonctionnalités des frames SDI.
LeftView.h, LeftView.cpp
    Ces fichiers contiennent la classe du frame gauche CLeftView, dérivée de
    CTreeView.
res\Toolbar.bmp
    Ce fichier bitmap permet de créer des images en mosaïque pour la barre d'outils.
    La barre d'outils et la barre d'état initiales sont créées dans la classe CMainFrame.
    Éditez cette bitmap de barre d'outils dans l'Éditeur de ressources, puis
    mettez à jour le tableau IDR_MAINFRAME TOOLBAR dans VedaDemoOGLMfcGui.rc pour ajouter
    des boutons de barre d'outils.
/////////////////////////////////////////////////////////////////////////////

AppWizard crée un type de document et une vue :

VedaDemoOGLMfcGuiDoc.h, VedaDemoOGLMfcGuiDoc.cpp - le document
    Ces fichiers contiennent votre classe CVedaDemoOGLMfcGuiDoc. Éditez-les pour ajouter vos
    propres données de document et pour implémenter l'enregistrement et le chargement de fichier
    (via CVedaDemoOGLMfcGuiDoc::Serialize).
    Le document comporte les chaînes suivantes :
        Extension de fichier :         avb
        ID du type de fichier :        VedaDemoOGLMfcGui.Document
        Légende du frame principal :   VedaDemoOGLMfcGui
        Nom du type de document :      VedaDemoOGLMfcG
        Nom du filtre :                VedaDemoOGLMfcGui Files (*.avb)
        Nouveau nom court de fichier : VedaDemoOGLMfcG
        Nom long du type de fichier :  VedaDemoOGLMfcGui.Document
VedaDemoOGLMfcGuiView.h, VedaDemoOGLMfcGuiView.cpp - la vue du document
    Ces fichiers contiennent votre classe CVedaDemoOGLMfcGuiView.
    Les objets CVedaDemoOGLMfcGuiView servent à afficher les objets CVedaDemoOGLMfcGuiDoc.
/////////////////////////////////////////////////////////////////////////////

Autres fonctionnalités :

Contrôles ActiveX
    L'application offre la prise en charge des contrôles ActiveX.
/////////////////////////////////////////////////////////////////////////////

Autres fichiers standard :

StdAfx.h, StdAfx.cpp
    Ces fichiers servent à générer un fichier d'en-tête précompilé (PCH),
    appelé VedaDemoOGLMfcGui.pch, et un fichier de types précompilés StdAfx.obj.

Resource.h
    Il s'agit du fichier d'en-tête standard, qui définit les ID des nouvelles ressources.
    Ce fichier est lu et mis à jour par Microsoft Visual C++.

/////////////////////////////////////////////////////////////////////////////

Autres remarques :

AppWizard utilise "TODO:" pour vous signaler les parties du code source
que vous devez ajouter ou personnaliser.

Si votre application utilise MFC dans une DLL partagée et que la langue de cette
application n'est pas la même que celle utilisée par le système d'exploitation, vous
devez copier les ressources localisées MFC70XXX.DLL correspondantes (situées sur le
CD-ROM Microsoft Visual C++ dans le répertoire Win\System) dans le répertoire system
ou system32 de votre ordinateur, puis renommer MFC70XXX.DLL en MFCLOC.DLL. ("XXX" est
l'abréviation de la langue. Par exemple, MFC70DEU.DLL contient les ressources en allemand.)
Si vous ne copiez pas la version localisée des ressources, certains éléments d'interface de
votre application seront affichés dans la langue du système d'exploitation.

/////////////////////////////////////////////////////////////////////////////
