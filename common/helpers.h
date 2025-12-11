//
// Created by WhoLeb on 28-Oct-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#define DEFAULT_COPY_DEFAULT_MOVE(x)                                                                                   \
    x(const x &) = default;                                                                                            \
    x &operator=(const x &) = default;                                                                                 \
    x(x &&) = default;                                                                                                 \
    x &operator=(x &&) = default;

#define NO_COPY_DEFAULT_MOVE(x)                                                                                        \
    x(const x &) = delete;                                                                                             \
    x &operator=(const x &) = delete;                                                                                  \
    x(x &&) = default;                                                                                                 \
    x &operator=(x &&) = default;

#define DEFAULT_COPY_NO_MOVE(x)                                                                                        \
    x(const x &) = default;                                                                                            \
    x &operator=(const x &) = default;                                                                                 \
    x(x &&) = delete;                                                                                                  \
    x &operator=(x &&) = delete;

#define NO_COPY_NO_MOVE(x)                                                                                             \
    x(const x &) = delete;                                                                                             \
    x &operator=(const x &) = delete;                                                                                  \
    x(x &&) = delete;                                                                                                  \
    x &operator=(x &&) = delete;
