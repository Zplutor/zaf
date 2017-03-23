#pragma once

class ContentPanel {
public:
    virtual ~ContentPanel() { }

    virtual void LoadContent() = 0;
};