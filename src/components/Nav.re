module Styles = {
  open Css;

  let container =
    style([
      position(`sticky),
      display(`flex),
      alignItems(`center),
      justifyContent(`spaceBetween),
      padding2(~v=`zero, ~h=`rem(1.5)),
      height(`rem(4.25)),
      marginBottom(`rem(-4.25)),
      width(`percent(100.)),
      zIndex(Theme.StackingIndex.zContent),
      media(
        Theme.MediaQuery.tablet,
        [
          position(`absolute),
          height(`rem(6.25)),
          padding2(~v=`zero, ~h=`rem(2.5)),
        ],
      ),
      media(
        Theme.MediaQuery.desktop,
        [height(`rem(7.)), padding2(~v=`zero, ~h=`rem(3.5))],
      ),
    ]);

  let spacer =
    style([
      height(`rem(4.25)),
      media(Theme.MediaQuery.tablet, [height(`rem(6.25))]),
      media(Theme.MediaQuery.desktop, [height(`rem(7.))]),
    ]);

  let spacerLarge =
    style([
      height(`rem(6.25)),
      media(Theme.MediaQuery.tablet, [height(`rem(9.5))]),
      media(Theme.MediaQuery.desktop, [height(`rem(14.))]),
    ]);

  let logo = style([cursor(`pointer), height(`rem(2.25))]);

  let nav =
    style([
      display(`flex),
      flexDirection(`column),
      position(`absolute),
      left(`zero),
      right(`zero),
      top(`rem(4.25)),
      width(`percent(90.)),
      margin2(~h=`auto, ~v=`zero),
      background(Theme.Colors.digitalBlack),
      media(Theme.MediaQuery.tablet, [top(`rem(6.25))]),
      media(
        Theme.MediaQuery.desktop,
        [
          position(`relative),
          top(`zero),
          width(`auto),
          zIndex(Theme.StackingIndex.zContent),
          flexDirection(`row),
          alignItems(`center),
          background(`none),
          margin2(~h=`zero, ~v=`zero),
        ],
      ),
    ]);

  let navLink =
    merge([
      Theme.Type.navLink,
      style([
        display(`flex),
        alignItems(`center),
        padding2(~v=`zero, ~h=`rem(1.5)),
        minHeight(`rem(5.5)),
        color(white),
        borderBottom(`px(1), `solid, Theme.Colors.digitalGray),
        hover([color(Theme.Colors.orange)]),
        media(
          Theme.MediaQuery.desktop,
          [
            position(`relative),
            marginRight(`rem(2.5)),
            width(`auto),
            height(`auto),
            padding(`zero),
            color(Theme.Colors.digitalBlack),
            border(`zero, `none, black),
          ],
        ),
      ]),
    ]);

  let navLabel = (selected, dark) => {
    merge([
      navLink,
      style([
        if (selected) {
          important(color(Theme.Colors.orange));
        } else {
          color(dark ? Theme.Colors.digitalBlack : white);
        },
      ]),
    ]);
  };

  let navGroup =
    style([
      width(`percent(100.)),
      top(`rem(4.5)),
      left(`rem(1.)),
      listStyleType(`none),
      color(white),
      background(Theme.Colors.digitalBlack),
      padding2(~h=`rem(1.5), ~v=`zero),
      selector(
        "> li",
        [
          display(`flex),
          alignItems(`center),
          width(`percent(100.)),
          height(`rem(5.5)),
          cursor(`pointer),
          borderBottom(`px(1), `solid, Theme.Colors.digitalGray),
          hover([color(Theme.Colors.orange)]),
        ],
      ),
      media(
        Theme.MediaQuery.desktop,
        [width(`rem(19.)), position(`absolute)],
      ),
    ]);

  let navToggle =
    style([
      cursor(`pointer),
      hover([color(Theme.Colors.orange)]),
      media(Theme.MediaQuery.desktop, [display(`none)]),
    ]);

  let hiddenToggle =
    style([
      display(`none),
      selector("+ label > #close-nav", [display(`none)]),
      selector("+ label > #open-nav", [display(`block)]),
      selector("~ nav", [display(`none)]),
      checked([
        selector("~ nav", [display(`flex)]),
        selector("+ label > #close-nav", [display(`block)]),
        selector("+ label > #open-nav", [display(`none)]),
      ]),
      media(
        Theme.MediaQuery.desktop,
        [selector("~ nav", [display(`flex)])],
      ),
    ]);

  let ctaContainer =
    style([
      padding2(~v=`rem(1.5), ~h=`rem(1.5)),
      media(Theme.MediaQuery.desktop, [padding(`zero)]),
    ]);

  let nav__item = selected =>
    merge([
      Theme.Type.navLink,
      style([
        flexGrow(1.),
        selected ? color(Theme.Colors.orange) : color(Theme.Colors.white),
      ]),
    ]);
};

module NavLink = {
  [@react.component]
  let make = (~href, ~label, ~dark) => {
    let currentItem = Context.HeaderNavCurrentItemContext.useCurrentItem();
    let isSelected = href === currentItem;
    <Next.Link href>
      <span className={Styles.navLabel(isSelected, dark)}>
        {React.string(label)}
      </span>
    </Next.Link>;
  };
};

module NavGroup = {
  /* Use these types to get the href prop from children */
  type href = {href: string};
  type childrenProps = {props: href};
  external asProps: React.element => childrenProps = "%identity";

  [@react.component]
  let make =
      (~label, ~children, ~active, ~switchGetStartedNavOpen, ~dark=false) => {
    let currentItem = Context.HeaderNavCurrentItemContext.useCurrentItem();
    let isSelected =
      children
      ->React.Children.toArray
      ->Belt.Array.keep(child => {
          let childProps = asProps(child);
          childProps.props.href === currentItem;
        })
      |> Array.length > 0;

    <>
      <span
        className={Styles.navLabel(isSelected, dark)}
        onClick={_ => switchGetStartedNavOpen()}>
        {React.string(label)}
      </span>
      {active ? <ul className=Styles.navGroup> children </ul> : React.null}
    </>;
  };
};

module NavGroupLink = {
  [@react.component]
  let make = (~icon, ~href, ~label) => {
    let currentItem = Context.HeaderNavCurrentItemContext.useCurrentItem();

    <Next.Link href>
      <li className={Styles.nav__item(currentItem === href)}>
        <Icon kind=icon size=2. />
        <Spacer width=1. />
        <span> {React.string(label)} </span>
        <Icon kind=Icon.ArrowRightSmall size=1.5 />
      </li>
    </Next.Link>;
  };
};

[@react.component]
let make =
    (
      ~isMobileNavOpen,
      ~switchMobileNavOpen,
      ~getStartedNavOpen,
      ~switchGetStartedNavOpen,
      ~dark=false,
    ) => {
  let router = Next.Router.useRouter();
  let (currentItem, setCurrentItem) = React.useState(() => router.asPath);

  let width = Hooks.useOnScreenWidth();

  let renderNav = () => {
    <nav className=Styles.nav>
      <Context.HeaderNavCurrentItemContext value=currentItem>
        <NavLink label="About" href="/about" dark />
        <NavLink label="Tech" href="/tech" dark />
        <NavGroup
          label="Get Started"
          active=getStartedNavOpen
          switchGetStartedNavOpen
          dark>
          <NavGroupLink icon=Icon.Box label="Overview" href="/get-started" />
          <NavGroupLink
            icon=Icon.NodeOperators
            label="Node Operators"
            href="/node-operators"
          />
          <NavGroupLink icon=Icon.Testnet label="Testnet" href="/testnet" />
          <NavGroupLink
            icon=Icon.GrantsProgram
            label="Grants"
            href="/grants"
          />
          <NavGroupLink
            icon=Icon.Documentation
            label="Documentation"
            href="/docs"
          />
        </NavGroup>
        <NavLink label="Community" href="/community" dark />
        <NavLink label="Blog" href="/blog" dark />
        <Spacer width=1.5 />
        <div className=Styles.ctaContainer>
          <Button
            href={`Internal("/genesis")}
            width={`rem(13.)}
            paddingX=1.
            dark={width < Theme.MediaQuery.desktopBreakpoint ? !dark : dark}>
            <img src="/static/img/promo-logo.svg" height="40" />
            <Spacer width=0.5 />
            <span> {React.string("Join Genesis Token Program")} </span>
          </Button>
        </div>
      </Context.HeaderNavCurrentItemContext>
    </nav>;
  };

  <header className=Styles.container>
    <Next.Link href="/">
      {dark
         ? <img
             src="/static/img/mina-wordmark-dark.svg"
             className=Styles.logo
           />
         : <img
             src="/static/img/mina-wordmark-light.svg"
             className=Styles.logo
           />}
    </Next.Link>
    <label onClick={_ => switchMobileNavOpen()} className=Styles.navToggle>
      {isMobileNavOpen
         ? <span id="close-nav"> <Icon kind=Icon.CloseMenu size=3. /> </span>
         : <span id="open-nav"> <Icon kind=Icon.BurgerMenu size=2. /> </span>}
    </label>
    /* Show nav if desktop or if mobile with the mobile nav open */
    {if (width > Theme.MediaQuery.desktopBreakpoint
         || width < Theme.MediaQuery.desktopBreakpoint
         && isMobileNavOpen) {
       renderNav();
     } else {
       React.null;
     }}
  </header>;
};
