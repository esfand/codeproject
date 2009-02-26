This is the source code for the Navigation Sidebar. I suggest you read my article and take note of the following code in order to understand and use my code.

The Necessary Code <===> This is from my Article

This code is to be put after you've made all the status bars and have updated the layout. First, we need to get the client rect, set our parameters that must be set before the window is created, and then create the window.

RECT ClientRect;
GetClientRect(&ClientRect);

// Set the button parameters. You must set these before you create the navwindow. If you don't, don't expect it to work ;)
m_navwindow.SetButtonHeight(31); // The button height is the height of the big buttons and the command bar
m_navwindow.SetSmallButtonWidth(24); // The small button width is the width of the little buttons on the command bar.

// Drop in our control.
m_hWndClient = m_navwindow.Create(m_hWnd, ClientRect, L"Nav_Main", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

Then we set the distance from the top that the buttons start popping back (so that you don't cover up anything important). This can be set at any place in the program, as it always defaults to zero if not set (in fact, you can edit the TabbedButtons so that it sets the popback differently based on what view it's showing). The following for loop is to define the windows and their HWNDs that are used by the tabbed buttons.

m_navwindow.SetPopBack(150); // Set the distance from the top of the client rect of the window where the buttons start popping back. 
	                            			// If not set or set to 0, the buttons will pop back when the buttons are pushed past the top of the window.


// For the sake of example, there is only one sample window for all the buttons. 
// If you want a different window for each button (you probably do), 
// you will want a different line and class for each window in your Mainframe.
// As these are for example, the following two lines and the for loop are to be erased and replaced with whatever you want.
button_view.resize(9);
button_view_top.resize(9);
for (size_t view_increment = 0; view_increment < button_view.size(); view_increment++)
{
	button_view[view_increment].Create(m_navwindow, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	button_view_top[view_increment].Create(m_navwindow.m_HorizontalSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		
	std::wostringstream vertical_caption;
	vertical_caption << L"Vertical " << view_increment;
	button_view[view_increment].window_caption = vertical_caption.str();
	button_view[view_increment].is_horizontal = false;
		
	std::wostringstream horizontal_caption;
	horizontal_caption << L"Horizontal " << view_increment;
	button_view_top[view_increment].window_caption = horizontal_caption.str();
	button_view_top[view_increment].is_horizontal = true;
	button_view_top[view_increment].pop_back_amount = m_navwindow.m_HorizontalSplitter.pop_back_y;
}
// Don't erase anything past here...it contains the rest of the parameters needed ;)

We then set the font of the buttons and then, most importantly, create them. The first argument is the button's caption, the second the imagepath it uses as a big button, the third the imagepath it uses as a button on the command bar, and the last two, respectively, are the HWNDs to what will appear on the right hand side across from the vertical bar and what will appear on the top side across the horizontal bar.

Then we set the splitter position of the vertical splitter to the chosen width and the default buttons to what we want to see...5 means that we will be seeing the first 4 buttons added with AddButtonWithView on top of a commandbar showing the other 5. For an alternate example, if you SetDefaultButtons(4), you will see 3 big buttons on top of a commandbar holding the other 6.

// Set the button font.
m_navwindow.SetButtonFont(L"Arial", 8, FontStyleBold);

// Add all of our buttons to our button view. Use a for loop if you have the necessary arrays of strings.
m_navwindow.AddButtonWithView(L"Print", L"RolloutIcons\\Print_2.tif", L"RolloutIcons\\Print_2_Small.tif", button_view[0], button_view_top[0]);
m_navwindow.AddButtonWithView(L"Add", L"RolloutIcons\\Add_2.tif", L"RolloutIcons\\Add_2_Small.tif", button_view[1], button_view_top[1]);
m_navwindow.AddButtonWithView(L"Edit", L"RolloutIcons\\Edit_2.tif", L"RolloutIcons\\Edit_2_Small.tif", button_view[2], button_view_top[2]);
m_navwindow.AddButtonWithView(L"Delete", L"RolloutIcons\\Delete_2.tif", L"RolloutIcons\\Delete_2_Small.tif", button_view[3], button_view_top[3]);
m_navwindow.AddButtonWithView(L"Search", L"RolloutIcons\\Search_2.tif", L"RolloutIcons\\Search_2_Small.tif", button_view[4], button_view_top[4]);
m_navwindow.AddButtonWithView(L"Back", L"RolloutIcons\\Back_2.tif", L"RolloutIcons\\Back_2_Small.tif", button_view[5], button_view_top[5]);
m_navwindow.AddButtonWithView(L"Forward", L"RolloutIcons\\Forward_2.tif", L"RolloutIcons\\Forward_2_Small.tif", button_view[6], button_view_top[6]);
m_navwindow.AddButtonWithView(L"Save", L"RolloutIcons\\Save_2.tif", L"RolloutIcons\\Save_2_Small.tif", button_view[7], button_view_top[7]);
m_navwindow.AddButtonWithView(L"Exit", L"RolloutIcons\\Exit_2.tif", L"RolloutIcons\\Exit_2_Small.tif", button_view[8], button_view_top[8]);

m_navwindow.SetSplitterPos(242); // Set this to be where the vertical splitter will be located on your window.
m_navwindow.SetDefaultButtons(5); // Set this to be 1 higher than the buttons you want to show initially. If none at all, put 1. If not even the command bar, put 0.

If you have any questions or comments, fill free to post on my article and I'll try to get back to you.